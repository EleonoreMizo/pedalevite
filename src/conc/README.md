# Conc

A library for lock-free inter-thread communications. It enables easy inter-tread communication and avoids locking critical threads with mutexes or other synchronisation objects.

Here are the main utility classes, from a user’s point of view:

- **`LockFreeCell`**: A template class to wrap a message object and use it in `LockFreeStack`, `LockFreeQueue` or `CellPool`.
- **`LockFreeStack`**: Lock-free stack (LIFO), completely thread-safe.
- **`LockFreeQueue`**: Lock-free queue (FIFO). Concurrent N to M threads access. It’s your main communication building block.
- **`CellPool`**: thread-safe storage for unused or recycled `LockFreeCell` objects.
- **`ObjPool`**: Like `CellPool`, but for any kind of object, providing additional control over creation.

These classes rely on atomic primitives (using Compare-And-Swap instructions). The **`Atomic*`** classes provide basic data storage, with `AtomicPtrIntPair` being the cornerstone of the previous data structures. **`AtomicIntOp`** is a helper wrapping a user-functor into a CAS loop to execute synchronized operations. There are some functor examples in the **`Aio*`** classes.

Some of these classes are probably less useful since the introduction of the `<atomic>` header in C++11, anyway the **`Interlocked`** class probably offers stronger lock-free guaranties, especially for 128-bit CAS required by `AtomicPtrIntPair` on 64-bit systems.