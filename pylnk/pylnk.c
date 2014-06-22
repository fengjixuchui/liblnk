/*
 * Python bindings module for liblnk (pylnk)
 *
 * Copyright (c) 2009-2014, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <types.h>

#if defined( HAVE_STDLIB_H ) || defined( HAVE_WINAPI )
#include <stdlib.h>
#endif

#include "pylnk.h"
#include "pylnk_error.h"
#include "pylnk_data_flags.h"
#include "pylnk_drive_types.h"
#include "pylnk_file.h"
#include "pylnk_file_attribute_flags.h"
#include "pylnk_file_object_io_handle.h"
#include "pylnk_libcerror.h"
#include "pylnk_libcstring.h"
#include "pylnk_liblnk.h"
#include "pylnk_python.h"
#include "pylnk_unused.h"

#if !defined( LIBLNK_HAVE_BFIO )
LIBLNK_EXTERN \
int liblnk_check_file_signature_file_io_handle(
     libbfio_handle_t *file_io_handle,
     liblnk_error_t **error );
#endif

/* The pylnk module methods
 */
PyMethodDef pylnk_module_methods[] = {
	{ "get_version",
	  (PyCFunction) pylnk_get_version,
	  METH_NOARGS,
	  "get_version() -> String\n"
	  "\n"
	  "Retrieves the version." },

	{ "check_file_signature",
	  (PyCFunction) pylnk_check_file_signature,
	  METH_VARARGS | METH_KEYWORDS,
	  "check_file_signature(filename) -> Boolean\n"
	  "\n"
	  "Checks if a file has a Windows Shortcut File (LNK) signature." },

	{ "check_file_signature_file_object",
	  (PyCFunction) pylnk_check_file_signature_file_object,
	  METH_VARARGS | METH_KEYWORDS,
	  "check_file_signature_file_object(file_object) -> Boolean\n"
	  "\n"
	  "Checks if a file has a Windows Shortcut File (LNK) signature using a file-like object." },

	{ "open",
	  (PyCFunction) pylnk_file_new_open,
	  METH_VARARGS | METH_KEYWORDS,
	  "open(filename, mode='r') -> Object\n"
	  "\n"
	  "Opens a file." },

	{ "open_file_object",
	  (PyCFunction) pylnk_file_new_open_file_object,
	  METH_VARARGS | METH_KEYWORDS,
	  "open_file_object(file_object, mode='r') -> Object\n"
	  "\n"
	  "Opens a file using a file-like object." },

	/* Sentinel */
	{ NULL, NULL, 0, NULL }
};

/* Retrieves the pylnk/liblnk version
 * Returns a Python object if successful or NULL on error
 */
PyObject *pylnk_get_version(
           PyObject *self PYLNK_ATTRIBUTE_UNUSED,
           PyObject *arguments PYLNK_ATTRIBUTE_UNUSED )
{
	const char *errors           = NULL;
	const char *version_string   = NULL;
	size_t version_string_length = 0;

	PYLNK_UNREFERENCED_PARAMETER( self )
	PYLNK_UNREFERENCED_PARAMETER( arguments )

	Py_BEGIN_ALLOW_THREADS

	version_string = liblnk_get_version();

	Py_END_ALLOW_THREADS

	version_string_length = libcstring_narrow_string_length(
	                         version_string );

	/* Pass the string length to PyUnicode_DecodeUTF8
	 * otherwise it makes the end of string character is part
	 * of the string
	 */
	return( PyUnicode_DecodeUTF8(
	         version_string,
	         (Py_ssize_t) version_string_length,
	         errors ) );
}

#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )

/* Checks if the file has a Windows Shortcut File (LNK) signature
 * Returns a Python object if successful or NULL on error
 */
PyObject *pylnk_check_file_signature(
           PyObject *self PYLNK_ATTRIBUTE_UNUSED,
           PyObject *arguments,
           PyObject *keywords )
{
	PyObject *exception_string    = NULL;
	PyObject *exception_traceback = NULL;
	PyObject *exception_type      = NULL;
	PyObject *exception_value     = NULL;
	PyObject *string_object       = NULL;
	libcerror_error_t *error      = NULL;
	static char *function         = "pylnk_check_file_signature";
	static char *keyword_list[]   = { "filename", NULL };
	const wchar_t *filename_wide  = NULL;
	const char *filename_narrow   = NULL;
	char *error_string            = NULL;
	int result                    = 0;

	PYLNK_UNREFERENCED_PARAMETER( self )

	/* Note that PyArg_ParseTupleAndKeywords with "s" will force Unicode strings to be converted to narrow character string.
	 * On Windows the narrow character strings contains an extended ASCII string with a codepage. Hence we get a conversion
	 * exception. We cannot use "u" here either since that does not allow us to pass non Unicode string objects and
	 * Python (at least 2.7) does not seems to automatically upcast them.
	 */
	if( PyArg_ParseTupleAndKeywords(
	     arguments,
	     keywords,
	     "|O",
	     keyword_list,
	     &string_object ) == 0 )
	{
		return( NULL );
	}
	PyErr_Clear();

	result = PyObject_IsInstance(
	          string_object,
	          (PyObject *) &PyUnicode_Type );

	if( result == -1 )
	{
		PyErr_Fetch(
		 &exception_type,
		 &exception_value,
		 &exception_traceback );

		exception_string = PyObject_Repr(
		                    exception_value );

		error_string = PyString_AsString(
		                exception_string );

		if( error_string != NULL )
		{
			PyErr_Format(
		         PyExc_RuntimeError,
			 "%s: unable to determine if string object is of type unicode with error: %s.",
			 function,
			 error_string );
		}
		else
		{
			PyErr_Format(
		         PyExc_RuntimeError,
			 "%s: unable to determine if string object is of type unicode.",
			 function );
		}
		Py_DecRef(
		 exception_string );

		return( NULL );
	}
	else if( result != 0 )
	{
		PyErr_Clear();

		filename_wide = (wchar_t *) PyUnicode_AsUnicode(
		                             string_object );
		Py_BEGIN_ALLOW_THREADS

		result = liblnk_check_file_signature_wide(
		          filename_wide,
		          &error );

		Py_END_ALLOW_THREADS

		if( result == -1 )
		{
			pylnk_error_raise(
			 error,
			 PyExc_IOError,
			 "%s: unable to check file signature.",
			 function );

			libcerror_error_free(
			 &error );

			return( NULL );
		}
		if( result != 0 )
		{
			return( Py_True );
		}
		return( Py_False );
	}
	PyErr_Clear();

	result = PyObject_IsInstance(
		  string_object,
		  (PyObject *) &PyString_Type );

	if( result == -1 )
	{
		PyErr_Fetch(
		 &exception_type,
		 &exception_value,
		 &exception_traceback );

		exception_string = PyObject_Repr(
				    exception_value );

		error_string = PyString_AsString(
				exception_string );

		if( error_string != NULL )
		{
			PyErr_Format(
		         PyExc_RuntimeError,
			 "%s: unable to determine if string object is of type string with error: %s.",
			 function,
			 error_string );
		}
		else
		{
			PyErr_Format(
		         PyExc_RuntimeError,
			 "%s: unable to determine if string object is of type string.",
			 function );
		}
		Py_DecRef(
		 exception_string );

		return( NULL );
	}
	else if( result != 0 )
	{
		PyErr_Clear();

		filename_narrow = PyString_AsString(
				   string_object );

		Py_BEGIN_ALLOW_THREADS

		result = liblnk_check_file_signature(
		          filename_narrow,
		          &error );

		Py_END_ALLOW_THREADS

		if( result == -1 )
		{
			pylnk_error_raise(
			 error,
			 PyExc_IOError,
			 "%s: unable to check file signature.",
			 function );

			libcerror_error_free(
			 &error );

			return( NULL );
		}
		if( result != 0 )
		{
			return( Py_True );
		}
		return( Py_False );
	}
	PyErr_Format(
	 PyExc_TypeError,
	 "%s: unsupported string object type",
	 function );

	return( NULL );
}

#else

/* Checks if the file has a Windows Shortcut File (LNK) signature
 * Returns a Python object if successful or NULL on error
 */
PyObject *pylnk_check_file_signature(
           PyObject *self PYLNK_ATTRIBUTE_UNUSED,
           PyObject *arguments,
           PyObject *keywords )
{
	libcerror_error_t *error    = NULL;
	static char *function       = "pylnk_check_file_signature";
	static char *keyword_list[] = { "filename", NULL };
	const char *filename        = NULL;
	int result                  = 0;

	PYLNK_UNREFERENCED_PARAMETER( self )

	if( PyArg_ParseTupleAndKeywords(
	     arguments,
	     keywords,
	     "|s",
	     keyword_list,
	     &filename ) == 0 )
	{
		return( NULL );
	}
	Py_BEGIN_ALLOW_THREADS

	result = liblnk_check_file_signature(
	          filename,
	          &error );

	Py_END_ALLOW_THREADS

	if( result == -1 )
	{
		pylnk_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to check file signature.",
		 function );

		libcerror_error_free(
		 &error );

		return( NULL );
	}
	if( result != 0 )
	{
		return( Py_True );
	}
	return( Py_False );
}

#endif /* defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER ) */

/* Checks if the file has a Windows Shortcut File (LNK) signature using a file-like object
 * Returns a Python object if successful or NULL on error
 */
PyObject *pylnk_check_file_signature_file_object(
           PyObject *self PYLNK_ATTRIBUTE_UNUSED,
           PyObject *arguments,
           PyObject *keywords )
{
	libcerror_error_t *error         = NULL;
	libbfio_handle_t *file_io_handle = NULL;
	PyObject *file_object            = NULL;
	static char *function            = "pylnk_check_file_signature_file_object";
	static char *keyword_list[]      = { "file_object", NULL };
	int result                       = 0;

	PYLNK_UNREFERENCED_PARAMETER( self )

	if( PyArg_ParseTupleAndKeywords(
	     arguments,
	     keywords,
	     "|O",
	     keyword_list,
	     &file_object ) == 0 )
	{
		return( NULL );
	}
	if( pylnk_file_object_initialize(
	     &file_io_handle,
	     file_object,
	     &error ) != 1 )
	{
		pylnk_error_raise(
		 error,
		 PyExc_MemoryError,
		 "%s: unable to initialize file IO handle.",
		 function );

		libcerror_error_free(
		 &error );

		goto on_error;
	}
	Py_BEGIN_ALLOW_THREADS

	result = liblnk_check_file_signature_file_io_handle(
	          file_io_handle,
	          &error );

	Py_END_ALLOW_THREADS

	if( result == -1 )
	{
		pylnk_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to check file signature.",
		 function );

		libcerror_error_free(
		 &error );

		goto on_error;
	}
	if( libbfio_handle_free(
	     &file_io_handle,
	     &error ) != 1 )
	{
		pylnk_error_raise(
		 error,
		 PyExc_MemoryError,
		 "%s: unable to free file IO handle.",
		 function );

		libcerror_error_free(
		 &error );

		goto on_error;
	}
	if( result != 0 )
	{
		return( Py_True );
	}
	return( Py_False );

on_error:
	if( file_io_handle != NULL )
	{
		libbfio_handle_free(
		 &file_io_handle,
		 NULL );
	}
	return( NULL );
}

/* Declarations for DLL import/export
 */
#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif

/* Initializes the pylnk module
 */
PyMODINIT_FUNC initpylnk(
                void )
{
	PyObject *module                               = NULL;
	PyTypeObject *data_flags_type_object           = NULL;
	PyTypeObject *drive_types_type_object          = NULL;
	PyTypeObject *file_type_object                 = NULL;
	PyTypeObject *file_attribute_flags_type_object = NULL;
	PyGILState_STATE gil_state                     = 0;

	/* Create the module
	 * This function must be called before grabbing the GIL
	 * otherwise the module will segfault on a version mismatch
	 */
	module = Py_InitModule3(
	          "pylnk",
	          pylnk_module_methods,
	          "Python liblnk module (pylnk)." );

	PyEval_InitThreads();

	gil_state = PyGILState_Ensure();

	/* Setup the file type object
	 */
	pylnk_file_type_object.tp_new = PyType_GenericNew;

	if( PyType_Ready(
	     &pylnk_file_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pylnk_file_type_object );

	file_type_object = &pylnk_file_type_object;

	PyModule_AddObject(
	 module,
	 "file",
	 (PyObject *) file_type_object );

	/* Setup the data flags type object
	 */
	pylnk_data_flags_type_object.tp_new = PyType_GenericNew;

	if( pylnk_data_flags_init_type(
	     &pylnk_data_flags_type_object ) != 1 )
	{
		goto on_error;
	}
	if( PyType_Ready(
	     &pylnk_data_flags_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pylnk_data_flags_type_object );

	data_flags_type_object = &pylnk_data_flags_type_object;

	PyModule_AddObject(
	 module,
	 "data_flags",
	 (PyObject *) data_flags_type_object );

	/* Setup the drive types type object
	 */
	pylnk_drive_types_type_object.tp_new = PyType_GenericNew;

	if( pylnk_drive_types_init_type(
	     &pylnk_drive_types_type_object ) != 1 )
	{
		goto on_error;
	}
	if( PyType_Ready(
	     &pylnk_drive_types_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pylnk_drive_types_type_object );

	drive_types_type_object = &pylnk_drive_types_type_object;

	PyModule_AddObject(
	 module,
	 "drive_types",
	 (PyObject *) drive_types_type_object );

	/* Setup the file attribute flags type object
	 */
	pylnk_file_attribute_flags_type_object.tp_new = PyType_GenericNew;

	if( pylnk_file_attribute_flags_init_type(
	     &pylnk_file_attribute_flags_type_object ) != 1 )
	{
		goto on_error;
	}
	if( PyType_Ready(
	     &pylnk_file_attribute_flags_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pylnk_file_attribute_flags_type_object );

	file_attribute_flags_type_object = &pylnk_file_attribute_flags_type_object;

	PyModule_AddObject(
	 module,
	 "file_attribute_flags",
	 (PyObject *) file_attribute_flags_type_object );

on_error:
	PyGILState_Release(
	 gil_state );
}

