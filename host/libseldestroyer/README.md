## libseldestroyer

TODO

### Thread safety
This library is partially thread-safe:
- You can call any function from different thread with the exception of...
- Calling the same function from two threads may bring to unexpected de-synchronization of the value. For instance, two concurrent access to V/I function may result in returing the same values for both calls

TODO