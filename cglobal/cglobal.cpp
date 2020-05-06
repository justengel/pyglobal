//
//cglobal: Try having a global dictionary hidden and inaccessible except through accessors.
//https://docs.python.org/3/extending/newtypes.html
//
// Helpful Links:
//   Additional Extending Python Help - https://docs.python.org/2.5/ext/node23.html
//   parsing-arguments - https://docs.python.org/3.4/c-api/arg.html
//   Extra variables - https://docs.python.org/2/c-api/structures.html#METH_CLASS
//   Packaging - https://packaging.python.org/guides/packaging-binary-extensions/
//   Py27 compiler - http://aka.ms/vcpython27
//   Py34 compiler - www.microsoft.com/download/details.aspx?id=8279
//   compilers - https://wiki.python.org/moin/WindowsCompilers
//

#include <Python.h>


typedef struct {
PyObject_HEAD

    private:
        PyObject *container;  // Should only be usable by this struct

    public:
        PyObject* init(void)
        {
            container = PyDict_New();
            Py_RETURN_NONE;
        }

        PyObject* kill(void)
        {
            Py_XDECREF(container);
            Py_RETURN_NONE;
        }

        PyObject* get_scope(PyObject *scope)
        {
            if (!PyDict_Contains(container, scope)){
                PyDict_SetItem(container, scope, PyDict_New());
            }
            return PyDict_GetItem(container, scope);
        }

        PyObject* get(PyObject *key, PyObject *default_, PyObject *scope)
        {
            PyObject *result;
            PyObject *d = get_scope(scope);

            result = PyDict_GetItem(d, key);
            if (result == NULL){
                return default_;
            } else {
                return result;
            }
        }

        PyObject* set(PyObject *key, PyObject *value, PyObject *scope)
        {
            PyObject *d = get_scope(scope);
            PyDict_SetItem(d, key, value);
            Py_RETURN_NONE;
        }

        PyObject* default_(PyObject *key, PyObject *value, PyObject *scope)
        {
            PyObject *d = get_scope(scope);
#if PY_MAJOR_VERSION >= 3
            PyDict_SetDefault(d, key, value);
#else
            if(!PyDict_Contains(d, key)){
                PyDict_SetItem(d, key, value);
            }
#endif
            Py_RETURN_NONE;
        }

        PyObject* clear(PyObject *key, PyObject *scope)
        {
            PyObject *d;
            if (key == Py_None){
                d = get_scope(scope);
                PyDict_DelItem(container, scope);
                Py_XDECREF(d);
            } else {
                PyDict_DelItem(get_scope(scope), key);
            }
            Py_RETURN_NONE;
        }
} GlobalSettings;


static PyObject *
GlobalSettings_get(GlobalSettings* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;
    PyObject *value = Py_None;
    PyObject *scope = Py_None;

    static char *kwlist[] = {"key", "default", "scope", NULL};

    //Parse the arguments
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "O|OO", kwlist,
                                      &key, &value, &scope))
        return NULL;

    return self->get(key, value, scope);
}


static PyObject *
GlobalSettings_set(GlobalSettings* self, PyObject *args, PyObject *kwds)
{
    PyObject *value;
    PyObject *key = Py_None;
    PyObject *scope = Py_None;

    static char *kwlist[] = {"key", "value", "scope", NULL};

    //Parse the arguments
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "OO|O", kwlist,
                                      &key, &value, &scope))
        return NULL;

    return self->set(key, value, scope);
}

static PyObject *
GlobalSettings_default(GlobalSettings* self, PyObject *args, PyObject *kwds)
{
    PyObject *value;
    PyObject *key = Py_None;
    PyObject *scope = Py_None;

    static char *kwlist[] = {"key", "value", "scope", NULL};

    //Parse the arguments
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "OO|O", kwlist,
                                      &key, &value, &scope))
        return NULL;

    return self->default_(key, value, scope);
}

static PyObject *
GlobalSettings_clear(GlobalSettings* self, PyObject *args, PyObject *kwds)
{
    PyObject *key = Py_None;
    PyObject *scope = Py_None;

    static char *kwlist[] = {"key", "scope", NULL};

    //Parse the arguments
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "O|O", kwlist,
                                      &key, &scope))
        return NULL;

    return self->clear(key, scope);
}

static void
GlobalSettings_dealloc(GlobalSettings* self)
{
    self->kill();
    Py_TYPE(self)->tp_free((PyObject*)self);
}


static PyObject *
GlobalSettings_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    GlobalSettings *self;

    self = (GlobalSettings *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->init();
    }

    return (PyObject *)self;
}


static int
GlobalSettings_init(GlobalSettings *self, PyObject *args, PyObject *kwds)
{
    return 0;
}


//static PyMemberDef GlobalSettings_members[] = {
////    {"first", T_OBJECT_EX, offsetof(GlobalSettings, first), 0,
////     "first name"},
//
//    {NULL}  /* Sentinel */
//};

static PyMethodDef GlobalSettings_methods[] = {
    {"get", (PyCFunction) GlobalSettings_get, METH_VARARGS | METH_KEYWORDS,
     "Return the stored value for the given key.\n"
     "\n"
     "Args:\n"
     "    key (str): Key to access the stored object with.\n"
     "    default (object)[None]: If the key does not exist return this default value.\n"
     "    scope (str)[None]: Additional key to prevent clashing.\n"
     "\n"
     "Returns:\n"
     "    value (object): Value or default value that was stored.\n"
    },
    {"set", (PyCFunction) GlobalSettings_set, METH_VARARGS | METH_KEYWORDS,
     "Set the stored value with the given key.\n"
     "\n"
     "Args:\n"
     "    key (str): Key to access the stored object with.\n"
     "    value (object): Store this object.\n"
     "    scope (str)[None]: Additional key to prevent clashing.\n"
    },
    {"default", (PyCFunction) GlobalSettings_default, METH_VARARGS | METH_KEYWORDS,
     "Set the default value for the given key.\n"
     "\n"
     "Args:\n"
     "    key (str): Key to access the stored object with.\n"
     "    value (object): Store this object.\n"
     "    scope (str)[None]: Additional key to prevent clashing.\n"
    },
    {"clear", (PyCFunction) GlobalSettings_clear, METH_VARARGS | METH_KEYWORDS,
     "Clear the given key or all storage for the scope if the given key is None.\n"
     "\n"
     "Args:\n"
     "    key (str)[None]: Key to access the stored object with.\n"
     "    scope (str)[None]: Additional key to prevent clashing.\n"
    },
    {NULL}  /* Sentinel */
};


//static PyTypeObject GlobalSettingsType = {
//    PyVarObject_HEAD_INIT(NULL, 0)
//    .tp_name = "cglobal.GlobalSettings",
//    .tp_doc = "Global settings object",
//    .tp_basicsize = sizeof(GlobalSettings),
//    .tp_itemsize = 0,
//    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
//    .tp_new = GlobalSettings_new,
//    .tp_init = (initproc) GlobalSettings_init,
//    .tp_dealloc = (destructor) GlobalSettings_dealloc,
////    .tp_members = GlobalSettings_members,
//    .tp_methods = GlobalSettings_methods,
//};
static PyTypeObject GlobalSettingsType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "cglobal.GlobalSetings",           /* tp_name */
    sizeof(GlobalSettings),            /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)GlobalSettings_dealloc,/* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "Global settings object",     /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    GlobalSettings_methods,            /* tp_methods */
    0,                         /* tp_members */
    0,  // GlobalSettings_members,          /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)GlobalSettings_init,     /* tp_init */
    0,                         /* tp_alloc */
    GlobalSettings_new,                /* tp_new */
};


static PyMethodDef cglobal_module_methods[] = {

    {NULL}  /* Sentinel */
};


//static PyModuleDef cglobalmodule = {
//    PyModuleDef_HEAD_INIT,
//    .m_name = "cglobal",
//    .m_doc = "Module with a C global settings structure with limited accessibility.",
//    .m_size = -1,
//};
static struct PyModuleDef cglobalmodule = {
    PyModuleDef_HEAD_INIT,
    "cglobal",
    "Module with a C global settings structure with limited accessibility.",
    -1,
    cglobal_module_methods, NULL, NULL, NULL, NULL
};


PyMODINIT_FUNC
PyInit_cglobal(void)
{
    PyObject *m;
    if (PyType_Ready(&GlobalSettingsType) < 0)
        return NULL;

    m = PyModule_Create(&cglobalmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&GlobalSettingsType);
    if (PyModule_AddObject(m, "GlobalSettings", (PyObject *) &GlobalSettingsType) < 0) {
        Py_DECREF(&GlobalSettingsType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}
