# DSA Explanations — MediFind

This document explains every Data Structure and Algorithm used in MediFind, its purpose, and its time/space complexity. It is intended to help understand and explain the code in a viva or project demonstration.

---

## 1. HashTable (Separate Chaining)

**File:** `include/dsa/HashTable.hpp`

**Used for:** Medicine ID cache in `InventoryService`. Provides near-instant lookup of a `Medicine` object by its integer ID without re-querying SQLite on every operation.

### How It Works

A HashTable stores key-value pairs in an array of "buckets". Each bucket is a linked list (chain) of entries that hashed to the same index.

```
Index 0: [ ]
Index 1: [ (id=1, Paracetamol) ] -> [ (id=11, Pantoprazole) ]
Index 2: [ (id=2, Amoxicillin) ]
...
Index N: [ ]
```

The hash function computes `index = key % capacity`. When two keys produce the same index (a "collision"), both entries are stored in the same chain.

When the load factor (items / capacity) exceeds 0.75, the table **rehashes**: it doubles capacity and redistributes all entries.

### Complexity

| Operation | Average | Worst Case |
|---|---|---|
| `insert(key, val)` | O(1) | O(n) — all keys in one chain |
| `find(key)` | O(1) | O(n) |
| `remove(key)` | O(1) | O(n) |
| `rehash()` | O(n) | O(n) |

**Space:** O(n)

### Viva Tip

> "We use a HashTable to cache medicines by their integer ID. When `InventoryService` needs to look up a medicine, it checks the cache first. Only if the key is missing does it query SQLite. This avoids repeated identical DB reads, which is especially useful when rendering the dashboard or processing a large sale."

---

## 2. Binary Search Tree (BST)

**File:** `include/dsa/BST.hpp`

**Used for:** Medicine name index in `InventoryService`. Allows prefix-based search (e.g. "Para" finds all medicines starting with "Para") ordered alphabetically.

### How It Works

A BST is a tree where each node stores a key. For any node:
- All keys in the **left subtree** are less than the node's key
- All keys in the **right subtree** are greater than the node's key

**In-order traversal** (left → node → right) visits all keys in alphabetical order.

**Prefix search** does an in-order traversal and collects nodes whose key starts with the given prefix.

```
         "Metformin"
        /            \
  "Ibuprofen"    "Omeprazole"
     /                \
"Cetirizine"       "Paracetamol"
```

### Complexity

| Operation | Average (balanced) | Worst (skewed) |
|---|---|---|
| `insert(key)` | O(log n) | O(n) |
| `find(key)` | O(log n) | O(n) |
| In-order traversal | O(n) | O(n) |
| Prefix search | O(n) | O(n) |

**Space:** O(n)

### Known Limitation

The current prefix search traverses the full in-order sequence and filters by prefix — this is O(n). A **Trie (prefix tree)** would give O(k) prefix search where k is the prefix length. The BST is used here because it also demonstrates ordering, which is valuable for the DSA demonstration.

---

## 3. Doubly Linked List

**File:** `include/dsa/LinkedList.hpp`

**Used for:** Storing audit log chain in the DSA demo layer; demonstrates bidirectional traversal.

### How It Works

Each node has:
- A `data` value
- A `next` pointer (to the next node)
- A `prev` pointer (to the previous node)

```
NULL <- [Head: item1] <-> [item2] <-> [item3: Tail] -> NULL
```

Nodes can be inserted or removed at either end in O(1).

### Complexity

| Operation | Time |
|---|---|
| `pushBack(val)` | O(1) |
| `pushFront(val)` | O(1) |
| `popFront()` | O(1) |
| `find(val)` | O(n) |
| `remove(val)` | O(n) |
| Traverse forward | O(n) |
| Traverse backward | O(n) |

**Space:** O(n)

---

## 4. Queue (FIFO)

**File:** `include/dsa/Queue.hpp`

**Used for:** Restock request processing order — requests are processed in the order they were submitted (First In, First Out).

### How It Works

Internally implemented using the custom `LinkedList`. New items join the **back** (enqueue), items leave from the **front** (dequeue).

```
Front [oldest request] -> [request2] -> [newest request] Back
Dequeue <-                                              <- Enqueue
```

### Complexity

| Operation | Time |
|---|---|
| `enqueue(val)` | O(1) |
| `dequeue()` | O(1) |
| `front()` | O(1) |
| `isEmpty()` | O(1) |

**Space:** O(n)

---

## 5. Stack (LIFO)

**File:** `include/dsa/Stack.hpp`

**Used for:** Operation history buffer — last operation is the first one available to review or undo.

### How It Works

Items are pushed to and popped from the **top** only. Internally implemented as a dynamic array.

```
     TOP
  [operation3]  <- push / pop here
  [operation2]
  [operation1]
    BOTTOM
```

### Complexity

| Operation | Time |
|---|---|
| `push(val)` | O(1) amortized |
| `pop()` | O(1) |
| `top()` | O(1) |
| `isEmpty()` | O(1) |

**Space:** O(n)

---

## 6. MergeSort

**File:** `include/dsa/Sorting.hpp`

**Used for:** Stable sorting of batch lists by expiry date (stable means equal keys keep their original order, which matters for FEFO tie-breaking).

### How It Works

Divide-and-conquer: recursively split the array in half, sort each half, then merge the two sorted halves.

```
[4, 2, 7, 1, 3]
  Split:  [4,2,7]  |  [1,3]
  Split:  [4,2]|[7]  |  [1]|[3]
  Merge:  [2,4,7]  |  [1,3]
  Merge:  [1,2,3,4,7]
```

### Complexity

| Case | Time | Space |
|---|---|---|
| Best | O(n log n) | O(n) |
| Average | O(n log n) | O(n) |
| Worst | O(n log n) | O(n) |

MergeSort is **always O(n log n)** regardless of input — this is why it is preferred for FEFO-critical sorting.

---

## 7. QuickSort

**File:** `include/dsa/Sorting.hpp`

**Used for:** In-place sorting of large batch lists by price for display purposes.

### How It Works

Choose a pivot, partition the array so all elements less than the pivot go left and all greater go right, then recursively sort each partition.

```
[5, 3, 8, 1, 4]   pivot=4
Partition: [3, 1] | [4] | [5, 8]
Recurse:   [1, 3] | [4] | [5, 8]
Result:    [1, 3, 4, 5, 8]
```

### Complexity

| Case | Time | Space |
|---|---|---|
| Best | O(n log n) | O(log n) |
| Average | O(n log n) | O(log n) |
| Worst | O(n²) — sorted input | O(n) |

QuickSort is **in-place** (O(log n) stack space) vs MergeSort's O(n) auxiliary space. It's faster in practice on average, but not stable.

---

## 8. Binary Search

**File:** `include/dsa/Sorting.hpp`

**Used for:** Searching a sorted array of batch IDs or prices.

### How It Works

On a sorted array, repeatedly halve the search space by comparing the target to the middle element.

```
Array: [1, 3, 5, 7, 9, 11], target=7
  mid=5 → 7>5 → search right half
  mid=7 → found at index 3
```

### Complexity

| Operation | Time |
|---|---|
| Search | O(log n) |

**Precondition:** Array must be sorted. MergeSort or QuickSort is called first.

---

## FEFO Algorithm — Step-by-Step Trace

**Scenario:** Selling 25 units of Paracetamol. Two batches exist:
- Batch A: expires 2026-03-01, qty=10
- Batch B: expires 2027-06-01, qty=30

**Today:** 2026-10-27

**Step 1:** Query eligible batches ordered by expiry ASC:
```sql
SELECT * FROM batches
WHERE medicine_id = ? 
  AND status IN ('AVAILABLE', 'NEAR_EXPIRY')
  AND expiry_date > '2026-10-27'
ORDER BY expiry_date ASC, id ASC;
```

> Note: Batch A (expires 2026-03-01) is already EXPIRED — excluded.

Result: Only Batch B (qty=30) is eligible.

**Step 2:** Allocate from Batch B: need 25, batch has 30 → allocate 25.

**Step 3:** Commit: deduct 25 from Batch B. New qty = 5.

**Result:** All 25 units come from Batch B (the only eligible batch). FEFO worked correctly — the expired Batch A was never touched.

**Second scenario** — Batch A NOT expired yet (qty=10, exp=2026-12-01), Batch B (qty=30, exp=2027-06-01), selling 15 units:

Step 1: Both eligible. Sort by expiry → Batch A first.
Step 2: Allocate 10 from Batch A (exhausted), 5 from Batch B.
Step 3: Batch A qty=0 (mark DEPLETED), Batch B qty=25.
