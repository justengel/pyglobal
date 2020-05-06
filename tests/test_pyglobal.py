
def test_simple():
    import pyglobal
    import settings

    assert pyglobal.get('abc') == 123
    assert pyglobal.get('qwerty') == ''
    assert pyglobal.get('Hello') == 'World!'

    assert pyglobal.get('SECRET_KEY') == '!!!CHANGE!!!'


def test_hack():
    import pyglobal

    pyglobal.set('SECRET_KEY', '*******')

    # Check if library can access the page global variable
    def get_glob(*args, **kwargs):
        global GLOBAL_SETTING
        try:
            len(GLOBAL_SETTING)
            raise AssertionError('Should not be able to access this object!')
        except (AttributeError, NameError):
            pass

    pyglobal.get = get_glob
    pyglobal.get('SECRET_KEY', None)

    # User can still manually grab the variable even though it is not defined in __all__.
    pyglobal.GLOBAL_SETTING
    try:
        len(pyglobal.GLOBAL_SETTING)
        raise AssertionError('Global Settings should not have a length')
    except (TypeError, AttributeError):
        pass
    try:
        for k in pyglobal.GLOBAL_SETTING:
            pass
        raise AssertionError('Global Settings should not be iterable')
    except (TypeError, AttributeError):
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

    test_simple()
    test_hack()

    # sys.argv.append('--run_memory')
    if '--run_memory' in sys.argv:
        run_memory()

    print('All pyglobal tests finished successfully!')
