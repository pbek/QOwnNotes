#include <git2.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    git_repository *repository = NULL;
    git_blob *blob = NULL;
    git_oid id;
    const char text[] = "QOwnNotes bundled libgit2 test\n";
    int result = 1;

    if (argc != 2 || git_libgit2_init() < 0) {
        return 1;
    }

    if (git_repository_init(&repository, argv[1], 1) < 0 ||
        git_blob_create_from_buffer(&id, repository, text, sizeof(text) - 1) < 0 ||
        git_blob_lookup(&blob, repository, &id) < 0) {
        const git_error *error = git_error_last();
        fprintf(stderr, "%s\n", error ? error->message : "libgit2 operation failed");
    } else if (git_blob_rawsize(blob) == sizeof(text) - 1 &&
               memcmp(git_blob_rawcontent(blob), text, sizeof(text) - 1) == 0) {
        result = 0;
    }

    git_blob_free(blob);
    git_repository_free(repository);
    git_libgit2_shutdown();
    return result;
}
