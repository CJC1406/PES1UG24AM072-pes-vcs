// ─── IMPLEMENTED ─────────────────────────────────────────────────────────────

int commit_create(const char *message, ObjectID *commit_id_out) {
    Commit commit;
    memset(&commit, 0, sizeof(Commit));

    // 1. Build the tree hierarchy from the current index
    if (tree_from_index(&commit.tree) != 0) {
        fprintf(stderr, "error: failed to create tree from index\n");
        return -1;
    }

    // 2. Identify the parent commit (if any) by reading HEAD
    if (head_read(&commit.parent) == 0) {
        commit.has_parent = 1;
    } else {
        commit.has_parent = 0; // This is the initial commit
    }

    // 3. Populate metadata (Author, Timestamp, Message)
    const char *author_name = pes_author(); 
    snprintf(commit.author, sizeof(commit.author), "%s", author_name);
    commit.timestamp = (uint64_t)time(NULL);
    snprintf(commit.message, sizeof(commit.message), "%s", message);

    // 4. Serialize the Commit structure into a text buffer
    void *buffer = NULL;
    size_t len = 0;
    if (commit_serialize(&commit, &buffer, &len) != 0) {
        return -1;
    }

    // 5. Write the serialized commit as an OBJ_COMMIT to the object store
    if (object_write(OBJ_COMMIT, buffer, len, commit_id_out) != 0) {
        free(buffer);
        return -1;
    }
    free(buffer);

    // 6. Update the current branch/HEAD to point to this new commit hash
    if (head_update(commit_id_out) != 0) {
        fprintf(stderr, "error: failed to update HEAD\n");
        return -1;
    }

    return 0;
}
