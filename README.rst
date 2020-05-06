===========
pyglobal
===========
Global accessors for things like settings. This could be used across applications and modules to share data.

This utility is to help easily share values across multiple modules and libraries without having to pass values around.


Example
=======

Simple example.

.. code-block:: python

    # settings.py
    import pyglobal

    pyglobal.set('abc', 123)
    pyglobal.default('qwerty', '')
    pyglobal.set('Hello', 'World!')

    pyglobal.default('SECRET_KEY', '!!!CHANGE!!!')  # Warning: Any library using pyglobal could then look for this value.

    pyglobal.set('DATABASE_URL', 'http://')  # Warning: Any library using pyglobal could then look for this value.


Then use the settings in another part of the project

.. code-block:: python

    # main.py
    import pyglobal
    import settings  # Set pyglobal values before using in other modules

    import db

    app = App(SECRET_KEY=pyglobal.get('SECRET_KEY'))


The sub module db.py doesn't have to know anything about settings.py and can be nested down packages.

.. code-block:: python

    # db.py
    import pyglobal

    db = Database(pyglobal.get('DATABASE_URL'))


Avoiding clashes with other projects
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Avoid clashes and add some small amount of security to your app with a scope

.. code-block:: python

    # settings.py
    import pyglobal
    import uuid

    app_scope = 'MYAPP=' + str(uuid.uuid4())

    def scope_get(key, default=None):
        return pyglobal.get(key, default, app_scope)

    def scope_set(key, value)
        return pyglobal.set(key, value, app_scope)

    pyglobal.scope_get = scope_get
    pyglobal.scope_set = scope_set

    pyglobal.set('SECRET_KEY', '!! Change !!', scope=app_scope)
    scope_set('DATABASE_URL', 'http://')


Once again you can use in other files, but you would also need the scope to access.

.. code-block:: python

    # main.py
    import pyglobal
    import settings

    secret = pyglobal.scope_get('SECRET_KEY')
    db_url = pyglobal.get('DATABASE_URL', scope=settings.app_scope)


At some point code can always be disassembled, so this isn't really secure.
All scopes are is another level of obfuscation and a way to avoid global names from clashing with other projects.
I ended up making a C++ extension which uses a private variable to make the underlying dictionary a little less
accessible.
