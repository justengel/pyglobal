import pyglobal


pyglobal.set('abc', 123)
pyglobal.default('qwerty', '')
pyglobal.set('Hello', 'World!')

pyglobal.default('SECRET_KEY', '!!!CHANGE!!!')  # Warning: Any library using pyglobal could then look for this value.

