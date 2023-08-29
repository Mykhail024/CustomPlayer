# MPRIS Server (mpris_server.hpp)

mpris_server.hpp is a header only C++ library that implements the MPRIS
specification on top of the sdbus-c++ library.

# Installing

Installation only requires copying the header file src/mpris_server.hpp. You
will need to install [sdbus-c++](https://github.com/Kistler-Group/sdbus-cpp/) too, refer to the library documentation for details.

# Using

First off, you might want to see the [MPRIS specification](https://specifications.freedesktop.org/mpris-spec/latest/index.html).

For usage it is best to see the single example `example/main.cpp`, which
showcases how easy it is to create a compliant player.

For a brief usage:

* There is only one class, `mpris::Server`. You can create one using
  `mpris::Server::make()`. It requires a single parameter: the name of the
  player.
* A Server object can also be created using the constructor, but then you're
  responsible for any creation error yourself.
* To implement MPRIS methods, call the `Server::on_*` methods, passing a
  function or a lambda.
* To set MPRIS properties, call the `Server::set_*` methods.
* To send the only signal MPRIS specifies, call
  `Server::send_seeked_signal()`.
* There are various properties that are set-up automatically for you. For
  example, the `CanQuit` property cannot be modified by users of the Server
  class; instead it is determined at runtime by checking if a callback
  has been setup (i.e. if `on_quit()` was called with a proper callback).
* Some other niceties include enums for `PlaybackStatus`, `LoopStatus` and
  metadata field entries.
* While the library will try to do some stuff for you automatically, such
  as checking `Rate` ranges or valid `TrackId` in `SetPosition`, one big
  exception to this is the `OpenUri` method: this simply gives you a string
  and it's up to you to implement necessary checks (this was done mostly
  not to have to write URI and MIME implementations too).
* Finally, you can disable the implementation, leaving only dummy functions,
  by defining the macro `NO_IMPL`. It is defined automatically on Windows (so
  that one may be able to use the library on Windows without needing to
  set-up sdbus-c++).
