# PES-VCS Lab Report



---

# Phase 1: Object Storage Foundation

## Objective

Implemented content-addressable object storage using SHA-256 hashes.

Implemented functions:

* `object_write()`
* `object_read()`

Main concepts covered:

* Content-addressable storage
* Directory sharding
* Atomic file writes
* Hash-based integrity verification

## Screenshot 1A

<img width="1600" height="425" alt="image" src="https://github.com/user-attachments/assets/21384eb6-5189-4e08-ba1e-5bf4da1eee31" />


## Screenshot 1B

<img width="1600" height="282" alt="image" src="https://github.com/user-attachments/assets/19ce4c9a-d12a-4bf7-9b69-4b96ea356596" />


## Observations

* Objects are stored using SHA-256 hashes.
* Files with identical content produce the same hash.
* Objects are split into directories using the first two hex characters of the hash.
* Corrupted objects can be detected by recomputing and verifying the hash.

---

# Phase 2: Tree Objects

## Objective

Implemented tree object generation from the index.

Implemented function:

* `tree_from_index()`

Main concepts covered:

* Directory representation
* Recursive tree structures
* Nested paths
* File modes and permissions

## Screenshot 2A

<img width="1278" height="346" alt="image" src="https://github.com/user-attachments/assets/8e10aa1e-f18b-4f2b-9f18-5efd34fd92d6" />


## Screenshot 2B

<img width="1600" height="418" alt="image" src="https://github.com/user-attachments/assets/9beda1b6-af59-4778-ab63-fe69ef2d9f31" />


## Observations

* Tree objects represent directories.
* Files are stored as blob entries.
* Subdirectories are stored as tree entries.
* Nested paths such as `src/main.c` automatically create subtree objects.
* Tree serialization is deterministic, so the same entries always produce the same hash.

---

# Code Files Completed So Far

* `object.c`
* `tree.c`

---

# Phase 3: The Index (Staging Area)

## Objective

Implemented index management for staging files before commits.

Implemented functions:

* `index_load()`
* `index_save()`
* `index_add()`

Main concepts covered:

* Text-based index file format
* Atomic writes using temporary files
* File metadata tracking
* Staging area management

## Screenshot 3A, 3B

<img width="1600" height="985" alt="image" src="https://github.com/user-attachments/assets/8faff08b-c843-4022-9338-c21326ea19f1" />


## Observations

* The index stores staged file information.
* Each entry contains mode, hash, modification time, file size, and path.
* The index is written atomically to avoid corruption.
* Files are sorted by path before saving.

---

# Phase 4: Commits and History

## Objective

Implemented commit creation and history tracking.

Implemented function:

* `commit_create()`

Main concepts covered:

* Commit objects
* Parent-child relationships between commits
* HEAD and branch references
* Snapshot-based version control

## Screenshot 4A

<img width="1440" height="1346" alt="image" src="https://github.com/user-attachments/assets/0bff3f83-4a63-40a7-8e7b-56f417502b0a" />


## Screenshot 4B

<img width="1514" height="596" alt="image" src="https://github.com/user-attachments/assets/33cc25be-6ff7-4de3-9467-f83b2e436b2f" />


## Screenshot 4C

<img width="1146" height="180" alt="image" src="https://github.com/user-attachments/assets/9293859c-8951-4996-97b0-19d9fe97dc06" />


## Observations

* Each commit stores a snapshot of the staged state.
* Commits point to a root tree object.
* Commits also point to their parent commit.
* HEAD tracks the current branch.

---

# Phase 5: Branching and Checkout Analysis

## Q5.1 How would `pes checkout <branch>` work?

To implement `pes checkout <branch>`, the system would first update `.pes/HEAD` so that it points to the selected branch reference.

Then it would read the commit hash stored in `.pes/refs/heads/<branch>`, load the corresponding commit object, and recursively load the tree objects associated with that commit.

The working directory would then be updated:

* Files present in the target branch but not in the current branch would be created.
* Files removed in the target branch would be deleted.
* Files that differ between branches would be overwritten.

This operation is complex because uncommitted local changes may conflict with the files from the target branch.

## Q5.2 How can dirty working directory conflicts be detected?

The system can compare three versions of each tracked file:

1. The file in the working directory
2. The staged file in the index
3. The committed file from the current branch

If the working directory file differs from the index version, then the user has local modifications.

If that same file also differs in the target branch, checkout should stop and report a conflict because switching branches would overwrite uncommitted work.

## Q5.3 What happens in detached HEAD state?

In detached HEAD state, `.pes/HEAD` directly stores a commit hash instead of a branch reference.

If the user creates new commits in this state, the commits still exist, but no branch points to them.

Those commits can later become unreachable if the user switches branches.

To recover them, the user could:

* Create a new branch pointing to the detached commit hash
* Use the commit hash directly to restore the commit
* Use a reflog-like mechanism if implemented

---

# Phase 6: Garbage Collection and Space Reclamation

## Q6.1 How can unreachable objects be deleted?

Garbage collection would begin from all branch references in `.pes/refs/heads/`.

Starting from each branch tip commit:

* Visit the commit object
* Visit its parent commits
* Visit the tree objects referenced by each commit
* Visit all blob and subtree objects inside each tree

All visited hashes are stored in a hash set for fast lookup.

After traversal, the system scans `.pes/objects/` and deletes any object whose hash is not present in the reachable set.

For a repository with 100,000 commits and 50 branches, the system may need to visit:

* Around 100,000 commit objects
* Around 100,000 tree objects
* Hundreds of thousands of blob objects depending on repository size

## Q6.2 Why is concurrent garbage collection dangerous?

Garbage collection can be dangerous if it runs while a commit is being created.

Example:

* A new blob object is written
* The tree object referencing it has not yet been written
* Garbage collection runs before the commit is complete
* GC sees the blob as unreachable and deletes it
* The commit later references a missing object

Git avoids this problem by:

* Using locks during critical operations
* Keeping temporary references during object creation
* Running garbage collection only when the repository is stable

---


---

# Code Files Completed

* `object.c`
* `tree.c`
* `index.c`
* `commit.c`

---

* Name: Chiranth J Chigateri
* SRN: PES1UG24AM072

