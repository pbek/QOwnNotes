"""Disposable, read-only Nextcloud/CalDAV fixtures. Never forwards requests."""

from contextlib import contextmanager
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
import json
import threading
from urllib.parse import urlsplit


CALENDAR = "/remote.php/dav/calendars/demo/everyday/"
TASKS = (
    ("Plan a small adventure", "Choose a trail and pack a notebook.", "Projects", 1),
    ("Collect ideas for the weekend", "Leave room for something unexpected.", "Personal", 5),
    ("Finish the reading journal", "Write down three ideas worth keeping.", "Reading", 3),
)
LABELS = [{"id": 1, "title": "Ideas", "color": "87bfff"},
          {"id": 2, "title": "Projects", "color": "b49ae8"}]
CARDS = [{"id": index, "title": title, "description": "## Next step\n\n" + description +
          "\n\n- [ ] Gather inspiration\n- [ ] Share a first draft",
          "stackId": 10, "boardId": 1, "order": index, "type": "plain",
          "owner": "demo", "archived": False,
          "duedate": "2026-11-20T12:00:00+00:00", "labels": LABELS,
          "assignedUsers": [], "deletedAt": 0}
         for index, (title, description, _, _) in enumerate(TASKS, 1)]
STACKS = [{"id": 10, "boardId": 1, "title": "Next steps", "order": 0, "cards": CARDS}]
BOARDS = [{"id": 1, "title": "Small adventures", "stacks": STACKS, "labels": LABELS}]


def calendar_response():
    responses = []
    for index in range(1, len(TASKS) + 1):
        responses.append(f"""<d:response><d:href>{CALENDAR}{index}.ics</d:href>
<d:propstat><d:prop><d:getetag>"demo-{index}"</d:getetag>
<d:getlastmodified>Wed, 11 Nov 2026 12:00:00 GMT</d:getlastmodified></d:prop>
<d:status>HTTP/1.1 200 OK</d:status></d:propstat></d:response>""")
    return ('<?xml version="1.0"?><d:multistatus xmlns:d="DAV:">' +
            "".join(responses) + "</d:multistatus>")


def task_ics(index):
    title, description, category, priority = TASKS[index - 1]
    return ("BEGIN:VCALENDAR\r\nVERSION:2.0\r\nPRODID:-//QOwnNotes//Screenshot demo//EN\r\n"
            f"BEGIN:VTODO\r\nUID:screenshot-task-{index}\r\nSUMMARY:{title}\r\n"
            f"DESCRIPTION:{description}\r\nCATEGORIES:{category}\r\nPRIORITY:{priority}\r\n"
            "DTSTAMP:20261111T120000Z\r\nDUE:20261120T120000Z\r\n"
            "STATUS:NEEDS-ACTION\r\nPERCENT-COMPLETE:0\r\nEND:VTODO\r\nEND:VCALENDAR\r\n")


class DemoHandler(BaseHTTPRequestHandler):
    def log_message(self, *_args):
        pass

    def respond(self, status, value, content_type="application/json"):
        body = (json.dumps(value) if content_type == "application/json" else value).encode()
        self.send_response(status)
        self.send_header("Content-Type", content_type + "; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self):
        url = urlsplit(self.path)
        # Qt also uses this server as its HTTP proxy. Reject every other origin.
        if url.netloc and url.netloc != f"127.0.0.1:{self.server.server_port}":
            self.respond(403, {"message": "External access disabled in screenshot demo"})
            return
        path = url.path
        self.server.requests.append((self.command, path))
        if path.endswith("/cloud/capabilities"):
            self.respond(200, {"ocs": {"meta": {"status": "ok", "statuscode": 100},
                         "data": {"capabilities": {}}}})
        elif path.endswith("/note/app_info"):
            self.respond(200, {"version": "26.11.0", "versions_app": True, "trash_app": True})
        elif path.endswith("/note/versions"):
            self.respond(200, {"file_name": "A little room for ideas.md", "versions": [
                {"humanReadableTimestamp": date, "data": self.server.note.replace(
                    "Your own space.", replacement),
                 "diffHtml": "<h2>A little room for ideas</h2><p>Your own " +
                             "<del>space</del> <ins>rules</ins>.</p>"}
                for date, replacement in (("11 November 2026, 10:30", "Your own rules."),
                                          ("10 November 2026, 16:45", "Your own pace."),
                                          ("9 November 2026, 09:15", "Your own notebook."))]})
        elif path.endswith("/note/trashed"):
            self.respond(200, {"directory": "Notes", "notes": [
                {"noteName": title, "fileName": title + ".md", "timestamp": 1794398400,
                 "dateString": "11 November 2026, 12:00", "data": f"# {title}\n\n{description}\n"}
                for title, description, _, _ in TASKS]})
        elif path.endswith("/boards"):
            self.respond(200, BOARDS)
        elif path.endswith("/boards/1/stacks"):
            self.respond(200, STACKS)
        elif path.endswith("/stacks/archived"):
            self.respond(200, [])
        elif path.startswith(CALENDAR) and path.endswith(".ics"):
            try:
                index = int(path[len(CALENDAR):-4])
                if not 1 <= index <= len(TASKS):
                    raise ValueError()
            except ValueError:
                self.respond(404, {})
                return
            self.respond(200, task_ics(index), "text/calendar")
        else:
            self.respond(404, {"message": "No screenshot fixture for this endpoint"})

    def do_REPORT(self):
        url = urlsplit(self.path)
        if (url.netloc and url.netloc != f"127.0.0.1:{self.server.server_port}") or url.path != CALENDAR:
            self.respond(403, {})
            return
        self.server.requests.append((self.command, url.path))
        self.respond(207, calendar_response(), "application/xml")

    def do_CONNECT(self):
        self.respond(403, {"message": "External access disabled in screenshot demo"})


@contextmanager
def demo_server(note):
    server = ThreadingHTTPServer(("127.0.0.1", 0), DemoHandler)
    server.note = note
    server.requests = []
    worker = threading.Thread(target=server.serve_forever, daemon=True)
    worker.start()
    try:
        yield server
    finally:
        server.shutdown()
        server.server_close()
        worker.join()
