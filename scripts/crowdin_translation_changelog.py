#! nix-shell --pure -i python3 -p python3

"""Generate and append a Crowdin translation changelog entry.

This script resolves the contribution window from the latest successful Crowdin
translation build to the current time by default, requests the Crowdin
top-members report for that window, derives contributed languages and member
handles from the report, prints the resulting changelog line, and appends it to
the latest release block in CHANGELOG.md.
"""

import argparse
import csv
import io
import json
import os
import re
import sys
import time
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from urllib import error, request


API_BASE_URL = "https://api.crowdin.com/api/v2"
DEFAULT_TIMEOUT_SECONDS = 30
REPORT_TIMEOUT_SECONDS = 180
POLL_INTERVAL_SECONDS = 2
DEFAULT_CHANGELOG_PATH = "CHANGELOG.md"
ENV_TOKEN_KEYS = (
    "CROWDIN_API_TOKEN",
    "CROWDIN_PERSONAL_TOKEN",
    "CROWDIN_TOKEN",
)


class CrowdinError(RuntimeError):
    pass


@dataclass
class BuildWindow:
    start: datetime
    end: datetime
    start_build_id: int | None
    end_build_id: int | None


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Generate a changelog line for Crowdin translation contributions. "
            "By default, the contribution window starts at the latest "
            "successful Crowdin translation build and ends at the current time."
        )
    )
    parser.add_argument(
        "--project-id",
        type=int,
        help="Override the Crowdin project id from crowdin.yaml",
    )
    parser.add_argument(
        "--token",
        help="Override the Crowdin API token (otherwise read from env or ~/.crowdin.yaml)",
    )
    parser.add_argument(
        "--config",
        default="crowdin.yaml",
        help="Path to the project Crowdin config file (default: crowdin.yaml)",
    )
    parser.add_argument(
        "--cli-config",
        default=str(Path.home() / ".crowdin.yaml"),
        help="Path to the Crowdin CLI config file (default: ~/.crowdin.yaml)",
    )
    parser.add_argument(
        "--from",
        dest="date_from",
        help="Start of the contribution window in ISO-8601 format",
    )
    parser.add_argument(
        "--to",
        dest="date_to",
        help="End of the contribution window in ISO-8601 format",
    )
    parser.add_argument(
        "--debug",
        action="store_true",
        help="Print the resolved build window and API progress to stderr",
    )
    parser.add_argument(
        "--changelog",
        default=DEFAULT_CHANGELOG_PATH,
        help=f"Path to the changelog file to update (default: {DEFAULT_CHANGELOG_PATH})",
    )
    return parser.parse_args()


def parse_simple_yaml(path: Path) -> dict[str, str]:
    if not path.exists():
        return {}

    values: dict[str, str] = {}
    for line in path.read_text(encoding="utf-8").splitlines():
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue

        match = re.match(r"^([A-Za-z0-9_]+):\s*(.*?)\s*$", stripped)
        if not match:
            continue

        key, value = match.groups()
        if not value or value in {"|", ">"}:
            continue

        if " #" in value:
            value = value.split(" #", 1)[0].rstrip()

        if len(value) >= 2 and value[0] == value[-1] and value[0] in {'"', "'"}:
            value = value[1:-1]

        values[key] = value

    return values


def parse_datetime(value: str) -> datetime:
    normalized = value.strip()
    if normalized.endswith("Z"):
        normalized = normalized[:-1] + "+00:00"

    parsed = datetime.fromisoformat(normalized)
    if parsed.tzinfo is None:
        parsed = parsed.replace(tzinfo=timezone.utc)

    return parsed.astimezone(timezone.utc)


def format_datetime(value: datetime) -> str:
    return value.astimezone(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


def get_project_id(config_path: Path, override: int | None) -> int:
    if override is not None:
        return override

    config = parse_simple_yaml(config_path)
    project_id = config.get("project_id")
    if not project_id:
        raise CrowdinError(f"No project_id found in {config_path}")

    try:
        return int(project_id)
    except ValueError as exception:
        raise CrowdinError(f"Invalid project_id in {config_path}: {project_id}") from exception


def get_token(cli_config_path: Path, override: str | None) -> str:
    if override:
        return override

    for env_key in ENV_TOKEN_KEYS:
        token = os.environ.get(env_key)
        if token:
            return token

    cli_config = parse_simple_yaml(cli_config_path)

    api_token_env = cli_config.get("api_token_env")
    if api_token_env:
        token = os.environ.get(api_token_env)
        if token:
            return token

    for key in ("api_token", "token", "personal_token"):
        token = cli_config.get(key)
        if token:
            return token

    env_names = ", ".join(ENV_TOKEN_KEYS)
    raise CrowdinError(
        "No Crowdin API token found. Pass --token, set one of "
        f"{env_names}, or configure api_token in {cli_config_path}"
    )


class CrowdinClient:
    def __init__(self, project_id: int, token: str, debug: bool = False):
        self.project_id = project_id
        self.token = token
        self.debug = debug

    def _request_json(self, method: str, url: str, payload: dict | None = None) -> dict:
        headers = {
            "Authorization": f"Bearer {self.token}",
            "Accept": "application/json",
        }
        data = None
        if payload is not None:
            headers["Content-Type"] = "application/json"
            data = json.dumps(payload).encode("utf-8")

        http_request = request.Request(url, data=data, headers=headers, method=method)
        try:
            with request.urlopen(http_request, timeout=DEFAULT_TIMEOUT_SECONDS) as response:
                return json.loads(response.read().decode("utf-8"))
        except error.HTTPError as exception:
            body = exception.read().decode("utf-8", errors="replace")
            raise CrowdinError(
                f"Crowdin API request failed with HTTP {exception.code}: {body}"
            ) from exception
        except error.URLError as exception:
            raise CrowdinError(f"Crowdin API request failed: {exception.reason}") from exception

    def get(self, endpoint: str) -> dict:
        return self._request_json("GET", f"{API_BASE_URL}{endpoint}")

    def post(self, endpoint: str, payload: dict) -> dict:
        return self._request_json("POST", f"{API_BASE_URL}{endpoint}", payload)

    def download_text(self, url: str) -> str:
        http_request = request.Request(url)
        try:
            with request.urlopen(http_request, timeout=DEFAULT_TIMEOUT_SECONDS) as response:
                return response.read().decode("utf-8-sig")
        except error.HTTPError as exception:
            body = exception.read().decode("utf-8", errors="replace")
            raise CrowdinError(
                f"Crowdin report download failed with HTTP {exception.code}: {body}"
            ) from exception
        except error.URLError as exception:
            raise CrowdinError(f"Crowdin report download failed: {exception.reason}") from exception

    def get_recent_finished_builds(self, limit: int = 10) -> list[dict]:
        response = self.get(f"/projects/{self.project_id}/translations/builds?limit={limit}")
        builds: list[dict] = []

        for item in response.get("data", []):
            build = item.get("data", {})
            if build.get("status") != "finished":
                continue

            build_time = None
            for key in ("finishedAt", "updatedAt", "createdAt"):
                value = build.get(key)
                if value:
                    build_time = parse_datetime(value)
                    break

            if build_time is None:
                continue

            build["_build_time"] = build_time
            builds.append(build)

        builds.sort(key=lambda build: build["_build_time"], reverse=True)
        return builds

    def resolve_build_window(
        self,
        explicit_from: str | None,
        explicit_to: str | None,
    ) -> BuildWindow:
        if explicit_from or explicit_to:
            if not explicit_from or not explicit_to:
                raise CrowdinError("--from and --to must be passed together")

            start = parse_datetime(explicit_from)
            end = parse_datetime(explicit_to)
            if start >= end:
                raise CrowdinError("--from must be earlier than --to")

            return BuildWindow(start=start, end=end, start_build_id=None, end_build_id=None)

        builds = self.get_recent_finished_builds(limit=10)
        if not builds:
            raise CrowdinError(
                "Need at least one successful Crowdin translation build to resolve the contribution window"
            )

        latest_build = builds[0]
        now = datetime.now(timezone.utc)
        if latest_build["_build_time"] >= now:
            raise CrowdinError(
                "The latest successful Crowdin translation build is not earlier than the current time"
            )

        return BuildWindow(
            start=latest_build["_build_time"],
            end=now,
            start_build_id=latest_build.get("id"),
            end_build_id=None,
        )

    def create_report(self, report_name: str, window: BuildWindow) -> str:
        payload = {
            "name": report_name,
            "schema": {
                "format": "csv",
                "dateFrom": format_datetime(window.start),
                "dateTo": format_datetime(window.end),
            },
        }
        response = self.post(f"/projects/{self.project_id}/reports", payload)
        report_id = response.get("data", {}).get("identifier")
        if not report_id:
            raise CrowdinError(f"Crowdin did not return a report id for report {report_name}")

        return str(report_id)

    def wait_for_report(self, report_id: str) -> None:
        deadline = time.time() + REPORT_TIMEOUT_SECONDS

        while time.time() < deadline:
            response = self.get(f"/projects/{self.project_id}/reports/{report_id}")
            data = response.get("data", {})
            status = str(data.get("status", "")).lower()

            if self.debug:
                print(
                    f"Report {report_id} status: {status or 'unknown'}",
                    file=sys.stderr,
                )

            if status == "finished":
                return

            if status in {"failed", "canceled", "cancelled"}:
                raise CrowdinError(f"Crowdin report {report_id} ended with status {status}")

            time.sleep(POLL_INTERVAL_SECONDS)

        raise CrowdinError(f"Timed out while waiting for Crowdin report {report_id}")

    def download_report_rows(self, report_id: str) -> list[dict[str, str]]:
        response = self.get(f"/projects/{self.project_id}/reports/{report_id}/download")
        url = response.get("data", {}).get("url")
        if not url:
            raise CrowdinError(f"Crowdin did not return a download url for report {report_id}")

        csv_text = self.download_text(url)
        reader = csv.DictReader(io.StringIO(csv_text))
        return list(reader)

    def get_report_rows(self, report_name: str, window: BuildWindow) -> list[dict[str, str]]:
        report_id = self.create_report(report_name, window)
        self.wait_for_report(report_id)
        return self.download_report_rows(report_id)


def normalize_key(value: str) -> str:
    return re.sub(r"[^a-z0-9]+", "", value.lower())


def parse_number(value: str | None) -> float | None:
    if value is None:
        return None

    candidate = value.strip().replace(",", "")
    if not candidate:
        return None

    try:
        return float(candidate)
    except ValueError:
        return None


def extract_member_handle(value: str) -> str:
    candidate = value.strip()
    if not candidate:
        return candidate

    match = re.search(r"\(([^()]+)\)\s*$", candidate)
    if match:
        handle = match.group(1).strip()
        if handle:
            return handle

    return candidate


def get_row_label(row: dict[str, str], kind: str) -> str | None:
    preferred_columns = {
        "languages": (
            "language",
            "languagename",
            "targetlanguage",
            "name",
        ),
        "members": (
            "username",
            "user",
            "login",
            "handle",
            "member",
            "fullname",
            "name",
        ),
    }

    normalized = {normalize_key(key): key for key in row.keys()}
    for candidate in preferred_columns[kind]:
        column = normalized.get(candidate)
        if column and row.get(column, "").strip():
            label = row[column].strip()
            if kind == "members":
                return extract_member_handle(label)

            return label

    for key, value in row.items():
        normalized_key = normalize_key(key)
        if kind == "languages" and "language" in normalized_key and value.strip():
            return value.strip()
        if kind == "members" and any(
            token in normalized_key
            for token in ("username", "login", "handle", "user", "member", "name")
        ) and value.strip():
            return extract_member_handle(value)

    return None


def score_row(row: dict[str, str]) -> float:
    preferred_metrics = (
        "translated",
        "approved",
        "proofread",
        "votes",
        "voted",
        "suggestions",
    )
    ignored_columns = (
        "id",
        "rank",
        "position",
        "language",
        "locale",
        "code",
        "user",
        "member",
        "name",
        "manager",
    )

    total = 0.0
    matched_metric = False

    for key, value in row.items():
        normalized_key = normalize_key(key)
        if any(token in normalized_key for token in ignored_columns):
            continue

        number = parse_number(value)
        if number is None:
            continue

        if any(token in normalized_key for token in preferred_metrics):
            total += number
            matched_metric = True

    if matched_metric:
        return total

    for key, value in row.items():
        normalized_key = normalize_key(key)
        if any(token in normalized_key for token in ignored_columns):
            continue

        number = parse_number(value)
        if number is not None:
            total += number

    return total


def collect_ranked_names(rows: list[dict[str, str]], kind: str) -> list[str]:
    scored_rows: list[tuple[str, float]] = []

    for row in rows:
        label = get_row_label(row, kind)
        if not label:
            continue

        score = score_row(row)
        if score <= 0:
            continue

        scored_rows.append((label, score))

    scored_rows.sort(key=lambda item: (-item[1], item[0].casefold()))

    unique_labels: list[str] = []
    seen: set[str] = set()
    for label, _score in scored_rows:
        normalized_label = label.casefold()
        if normalized_label in seen:
            continue

        seen.add(normalized_label)
        unique_labels.append(label)

    return unique_labels


def split_language_labels(value: str) -> list[str]:
    labels: list[str] = []
    for item in re.split(r"[,;\n]+", value):
        label = item.strip()
        if not label:
            continue

        labels.append(label)

    return labels


def get_row_languages(row: dict[str, str]) -> list[str]:
    preferred_columns = (
        "languages",
        "language",
        "languagename",
        "targetlanguage",
    )

    normalized = {normalize_key(key): key for key in row.keys()}
    for candidate in preferred_columns:
        column = normalized.get(candidate)
        if column and row.get(column, "").strip():
            return split_language_labels(row[column])

    for key, value in row.items():
        if "language" in normalize_key(key) and value.strip():
            return split_language_labels(value)

    return []


def collect_languages(rows: list[dict[str, str]]) -> list[str]:
    scored_rows: list[tuple[dict[str, str], float]] = []

    for row in rows:
        score = score_row(row)
        if score <= 0:
            continue

        scored_rows.append((row, score))

    scored_rows.sort(key=lambda item: -item[1])

    unique_labels: list[str] = []
    seen: set[str] = set()
    for row, _score in scored_rows:
        for label in get_row_languages(row):
            normalized_label = label.casefold()
            if normalized_label in seen:
                continue

            seen.add(normalized_label)
            unique_labels.append(label)

    return unique_labels


def build_changelog_line(languages: list[str], members: list[str]) -> str:
    language_list = ", ".join(languages)
    if members:
        return f"- Added more {language_list} translation (thank you, {', '.join(members)})"

    return f"- Added more {language_list} translation"


def append_to_latest_release(changelog_path: Path, changelog_line: str) -> bool:
    if not changelog_path.exists():
        raise CrowdinError(f"Changelog file not found: {changelog_path}")

    lines = changelog_path.read_text(encoding="utf-8").splitlines(keepends=True)

    latest_release_start = None
    for index, line in enumerate(lines):
        if line.startswith("## "):
            latest_release_start = index
            break

    if latest_release_start is None:
        raise CrowdinError(f"No release heading found in {changelog_path}")

    latest_release_end = len(lines)
    for index in range(latest_release_start + 1, len(lines)):
        if lines[index].startswith("## "):
            latest_release_end = index
            break

    existing_lines = {line.strip() for line in lines[latest_release_start:latest_release_end]}
    if changelog_line in existing_lines:
        return False

    insert_at = latest_release_end
    while insert_at > latest_release_start and not lines[insert_at - 1].strip():
        insert_at -= 1

    new_lines = [changelog_line + "\n"]
    if insert_at == latest_release_end or lines[insert_at].startswith("## "):
        new_lines.append("\n")

    lines[insert_at:insert_at] = new_lines
    changelog_path.write_text("".join(lines), encoding="utf-8")
    return True


def main() -> int:
    args = parse_args()

    try:
        project_id = get_project_id(Path(args.config), args.project_id)
        token = get_token(Path(args.cli_config), args.token)
        client = CrowdinClient(project_id=project_id, token=token, debug=args.debug)
        window = client.resolve_build_window(args.date_from, args.date_to)

        if args.debug:
            build_details = []
            if window.start_build_id is not None:
                build_details.append(f"from build {window.start_build_id}")
            if window.end_build_id is not None:
                build_details.append(f"to build {window.end_build_id}")

            suffix = ""
            if build_details:
                suffix = f" ({', '.join(build_details)})"

            print(
                f"Using contribution window {format_datetime(window.start)} -> {format_datetime(window.end)}{suffix}",
                file=sys.stderr,
            )

        member_rows = client.get_report_rows("top-members", window)

        languages = collect_languages(member_rows)
        members = collect_ranked_names(member_rows, "members")

        if not languages:
            print(
                "No translation contributions found for the resolved Crowdin build window.",
                file=sys.stderr,
            )
            return 0

        changelog_line = build_changelog_line(languages, members)
        changelog_updated = append_to_latest_release(Path(args.changelog), changelog_line)

        if args.debug:
            action = "Updated" if changelog_updated else "Skipped updating"
            print(f"{action} {args.changelog} with: {changelog_line}", file=sys.stderr)

        print(changelog_line)
        return 0
    except CrowdinError as exception:
        print(f"Error: {exception}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
