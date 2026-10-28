/**
 * MediFind — Interactive DSA Diagnostic Engines
 * Accurately simulates the custom C++ data structures running in MediFind:
 *  - HashTable<int, Medicine> with separate chaining
 *  - BST with in-order prefix matching
 *  - FEFO Priority Queue (Earliest Expiry First)
 */

class SimHashTable {
  constructor(capacity = 16) {
    this.capacity = capacity;
    this.buckets = Array.from({ length: capacity }, () => []);
  }

  hash(key) {
    return Math.abs(key) % this.capacity;
  }

  insert(key, value) {
    const idx = this.hash(key);
    const bucket = this.buckets[idx];
    for (let entry of bucket) {
      if (entry.key === key) {
        entry.value = value;
        return;
      }
    }
    bucket.push({ key, value });
  }

  render(containerId) {
    const container = document.getElementById(containerId);
    if (!container) return;
    container.innerHTML = '';

    this.buckets.forEach((b, idx) => {
      const div = document.createElement('div');
      div.className = `hash-bucket ${b.length > 0 ? 'filled' : ''}`;
      div.innerHTML = `<strong>B[${idx}]</strong>: ${b.length === 0 ? '<span class="text-muted">empty</span>' : b.map(e => `[#${e.key}:${e.value.name.split(' ')[0]}]`).join('&rarr;')}`;
      container.appendChild(div);
    });
  }
}

class BSTNode {
  constructor(name, medId) {
    this.name = name;
    this.medId = medId;
    this.left = null;
    this.right = null;
  }
}

class SimBST {
  constructor() {
    this.root = null;
  }

  insert(name, medId) {
    const newNode = new BSTNode(name, medId);
    if (!this.root) {
      this.root = newNode;
      return;
    }
    let cur = this.root;
    while (true) {
      if (name.toLowerCase() < cur.name.toLowerCase()) {
        if (!cur.left) {
          cur.left = newNode;
          break;
        }
        cur = cur.left;
      } else {
        if (!cur.right) {
          cur.right = newNode;
          break;
        }
        cur = cur.right;
      }
    }
  }

  inOrderTraversal(node = this.root, results = []) {
    if (!node) return results;
    this.inOrderTraversal(node.left, results);
    results.push({ name: node.name, id: node.medId });
    this.inOrderTraversal(node.right, results);
    return results;
  }

  findByPrefix(prefix) {
    const all = this.inOrderTraversal();
    if (!prefix) return all;
    const p = prefix.toLowerCase();
    return all.filter(item => item.name.toLowerCase().startsWith(p));
  }
}

// Global simulation instances
window.mediDsa = {
  hashTable: new SimHashTable(16),
  bst: new SimBST()
};
