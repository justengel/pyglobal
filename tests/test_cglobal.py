
def test_readme_scope():
    import pyglobal
    import uuid

    app_scope = 'MYAPP=' + str(uuid.uuid4())

    def scope_get(key, default=None):
        return pyglobal.get(key, default, app_scope)

    def scope_set(key, value):
        return pyglobal.set(key, value, app_scope)

    # Setting a new module function works
    pyglobal.scope_get = scope_get
    pyglobal.scope_set = scope_set

    pyglobal.set('SECRET_KEY', '!! Change !!', scope=app_scope)
    scope_set('DATABASE_URL', 'http://')


def test_cglobal_access():
    from pyglobal.cglobal import GlobalSettings

    s = GlobalSettings()

    s.set('Hello', 1)
    assert s.get('Hello') == 1
    assert 'cglobal' in str(s)


def test_cglobal_protection():
    # https://stackoverflow.com/questions/35346835/how-to-access-to-c-global-variable-structure-by-python-and-ctype

    import ctypes
    import inspect
    from pyglobal.cglobal import GlobalSettings

    s = GlobalSettings()
    attrs = dir(s)
    assert '__iter__' not in attrs
    assert '__contains__' not in attrs
    assert 'keys' not in attrs
    assert 'values' not in attrs
    assert 'items' not in attrs

    try:
        len(s.__container)
        raise AssertionError('Global Settings should not have a length')
    except (TypeError, AttributeError):
        pass
    try:
        for k in s.__container:
            pass
        raise AssertionError('Global Settings should not be iterable')
    except (TypeError, AttributeError):
        pass
    try:
        len(s.container)
        raise AssertionError('Global Settings should not have a length')
    except (TypeError, AttributeError):
        pass
    try:
        for k in s.container:
            pass
        raise AssertionError('Global Settings should not be iterable')
    except (TypeError, AttributeError):
        pass

    # Ctypes
    try:
        p = ctypes.pointer(s)
        raise AssertionError('Should not be able to get pointer.')
    except TypeError:
        pass


def run_memory():
    # Check your memory usage. It should not go up continuously.
    import pyglobal

    while True:
        pyglobal.default('default', 'oi')
        pyglobal.set('SECRET_KEY', "Hello World!")
        pyglobal.set('Other', {'a': 1, "b": 2})
        pyglobal.set('SECRET_KEY', "Hello World!", scope='MyScope')
        pyglobal.get('SECRET_KEY')
        pyglobal.get('SECRET_KEY', scope='MyScope')


if __name__ == '__main__':
    import sys

    test_readme_scope()
    test_cglobal_access()
    test_cglobal_protection()

    # sys.argv.append('--run_memory')
    if '--run_memory' in sys.argv:
        run_memory()

    print('All cglobal tests finished successfully!')
