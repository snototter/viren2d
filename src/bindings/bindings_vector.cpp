#include <sstream>
#include <stdexcept>

#include <pybind11/operators.h>
#include <pybind11/numpy.h>

#include <bindings/binding_helpers.h>
#include <werkzeugkiste/geometry/vector.h>


namespace py = pybind11;

namespace viren2d {
namespace bindings {

//------------------------------------------------- Vector from tuple
template<typename _Tp, int dim> inline
werkzeugkiste::geometry::Vec<_Tp, dim> VecFromTupleOrList(
    const py::tuple &tpl, const char *type) {
  using VC = werkzeugkiste::geometry::Vec<_Tp, dim>;
  if (tpl.empty()) {
    return VC();
  }

  if (tpl.size() != dim) {
    std::ostringstream s;
    s << "Cannot cast " << type << " with " << tpl.size()
      << " elements to `viren2d." << VC::TypeName() << "`!";
    throw std::invalid_argument(s.str());
  }

  VC vec;
  for (int i = 0; i < dim; ++i)
    vec.val[i] = tpl[i].cast<_Tp>();
  return vec;
}


/// The explicit VecFromList(py::list) is needed to allow pickling, as we
/// can only bind to the exact same type returned by __getstate__, see
/// VecToList().
template<typename _Tp, int dim>
werkzeugkiste::geometry::Vec<_Tp, dim> VecFromList(const py::list &lst) {
  return VecFromTupleOrList<_Tp, dim>(lst.cast<py::tuple>(), "list");
}


template<typename _Tp, int dim>
werkzeugkiste::geometry::Vec<_Tp, dim> VecFromTuple(const py::tuple &tpl) {
  return VecFromTupleOrList<_Tp, dim>(tpl, "tuple");
}


/// To support pickling, it is easier to serialize the vector
/// as a list. The other option would be to have explicit specializations
/// for each type and dimensionality, so we can create a py::tuple via
/// py::make_tuple...
template<typename _Tp, int dim>
py::list VecToList(const werkzeugkiste::geometry::Vec<_Tp, dim> &vec) {
  py::list lst;
  for (int i = 0; i < dim; ++i)
    lst.append(vec.val[i]);
  return lst;
}



/// Actual typed casting, called from VecFromArray()
template<typename _ATp, typename _VTp, int dim> inline
werkzeugkiste::geometry::Vec<_VTp, dim> VecFromArrayT(const py::array &arr) {
  using VC = werkzeugkiste::geometry::Vec<_VTp, dim>;

  const int rows = arr.shape(0);
  const int cols = (arr.ndim() > 1) ? arr.shape(1) : 1;
  const int channels = (arr.ndim() == 3) ? arr.shape(2) : 1;
  const int row_stride = arr.strides(0) / sizeof(_ATp);
  const int col_stride = (arr.ndim() > 1) ? (arr.strides(1) / sizeof(_ATp)) : 1;
  const int channel_stride = (arr.ndim() == 3) ? (arr.strides(2) / sizeof(_ATp)) : 1;

  if ((rows * cols * channels) != dim) {
    std::ostringstream s;
    s << "Cannot cast array with " << arr.size() << " elements to `viren2d."
      << VC::TypeName() << "`!";
    throw std::invalid_argument(s.str());
  }

  VC vec;
  int vec_idx = 0;
  const _ATp *arr_row_ptr = reinterpret_cast<const _ATp*>(arr.data());
  for (int row = 0; row < rows; ++row) {
    const _ATp *arr_col_ptr = arr_row_ptr;
    for (int col = 0; col < cols; ++col) {
      const _ATp *arr_chan_ptr = arr_col_ptr;
      for (int channel = 0; channel < channels; ++channel) {
        vec[vec_idx++] = static_cast<_VTp>(*arr_chan_ptr);
        arr_chan_ptr += channel_stride;
      }
      arr_col_ptr += col_stride;
    }
    arr_row_ptr += row_stride;
  }
  return vec;
}


/// Allows casting a numpy.ndarray (view) with DIM elements to a
/// DIM-dimensional Vec<>
template<typename _Tp, int dim> inline
werkzeugkiste::geometry::Vec<_Tp, dim> VecFromArray(const py::array &arr) {
  using VC = werkzeugkiste::geometry::Vec<_Tp, dim>;

  if (arr.size() == 0) {
    return VC();
  }
//FIXME extend imagebuffer conversion by:
  // 1) int8, bool (to uchar)
  // 2) non-c-contig --> iterate pixel by pixel
  const py::dtype dtype = arr.dtype();
  if (dtype.is(py::dtype::of<uint8_t>())) {
    return VecFromArrayT<uint8_t, _Tp, dim>(arr);
  } else if (dtype.is(py::dtype::of<int8_t>())) {
    return VecFromArrayT<int8_t, _Tp, dim>(arr);
  } else if (dtype.is(py::dtype::of<int16_t>())) {
    return VecFromArrayT<int16_t, _Tp, dim>(arr);
  } else if (dtype.is(py::dtype::of<uint16_t>())) {
    return VecFromArrayT<uint16_t, _Tp, dim>(arr);
  } else if (dtype.is(py::dtype::of<int32_t>())) {
    return VecFromArrayT<int32_t, _Tp, dim>(arr);
  } else if (dtype.is(py::dtype::of<uint32_t>())) {
    return VecFromArrayT<uint32_t, _Tp, dim>(arr);
  } else if (dtype.is(py::dtype::of<int64_t>())) {
    return VecFromArrayT<int64_t, _Tp, dim>(arr);
  } else if (dtype.is(py::dtype::of<uint64_t>())) {
    return VecFromArrayT<uint64_t, _Tp, dim>(arr);
  } else if (dtype.is(py::dtype::of<float>())) {
    return VecFromArrayT<float, _Tp, dim>(arr);
  } else if (dtype.is(py::dtype::of<double>())) {
    return VecFromArrayT<double, _Tp, dim>(arr);
  } else if (dtype.is(py::dtype::of<bool>())) {
    return VecFromArrayT<bool, _Tp, dim>(arr);
  } else {
    std::string s("Incompatible `dtype` (");
    s += py::cast<std::string>(dtype.attr("name"));
    s += ") to construct a `viren2d." + VC::TypeName() + "`!";
    throw std::invalid_argument(s);
  }
}


/// Generic casting: from tuple/list/ndarray to Vec<>
template<typename _Tp, int dim>
werkzeugkiste::geometry::Vec<_Tp, dim> VecFromPyObject(const py::object &object) {
  using VC = werkzeugkiste::geometry::Vec<_Tp, dim>;

  if (py::isinstance<py::tuple>(object)) {
    return VecFromTupleOrList<_Tp, dim>(object.cast<py::tuple>(), "tuple");
  } else if (py::isinstance<py::list>(object)) {
    return VecFromTupleOrList<_Tp, dim>(object.cast<py::tuple>(), "list");
  } else if (py::isinstance<py::array>(object)) {
    return VecFromArray<_Tp, dim>(object.cast<py::array>());
  } else {
    const std::string type = py::cast<std::string>(object.attr("__class__").attr("__name__"));
    std::ostringstream s;
    s << "Cannot cast `" << type << "` to `viren2d." << VC::TypeName()
      << "`. Only tuple, list and numpy.ndarray is supported!";
    throw std::invalid_argument(s.str());
  }

  const py::tuple tpl = object.cast<py::tuple>();
  if (!tpl.empty() && tpl.size() != dim) {
    std::ostringstream s;
    s << "Cannot create `viren2d." << VC::TypeName()
      << "` from " << tpl.size() << " values, expected "
      << dim << "!";
    throw std::invalid_argument(s.str());
  }

  if (tpl.empty())
    return VC();

  VC vec;
  for (int i = 0; i < dim; ++i)
    vec.val[i] = tpl[i].cast<_Tp>();
  return vec;
}


/// Returns the vector's type name to be used in docstrings, optionally
/// refering to the python built-in to be used by the cross-referencing within
/// our RTD documentation.
template<typename _Tp>
const char *PyTypeName(bool rst);

template<> const char *PyTypeName<double>(bool rst) {
  if (rst) {
    return ":class:`float`";
  } else {
    return "float:";
  }
}

template<> const char *PyTypeName<int>(bool rst) {
  if (rst) {
    return ":class:`int`";
  } else {
    return "int:";
  }
}


/// Needed to implicitly cast a Vec<> to a numpy array
template<typename _Tp, int dim>
py::buffer_info VecInfo(werkzeugkiste::geometry::Vec<_Tp, dim> &vec) {
  return py::buffer_info(
      &vec[0],
      sizeof(_Tp), // Size of each element
      py::format_descriptor<_Tp>::format(), // Python struct-style format descriptor
      1,  // The buffer is 1-dimensional
      { dim }, // Buffer dimensions
      { sizeof(_Tp) } // Stride (in bytes) per dimension
  );
}


template<typename _Tp, int dim>
void RegisterVec(py::module &m) {
  using VC = werkzeugkiste::geometry::Vec<_Tp, dim>;
  std::ostringstream doc;
  doc << "A " << dim
      << "D vector of " << PyTypeName<_Tp>(true) << " coordinates.";
  py::class_<VC> vec_cls(
        m, VC::TypeName().c_str(), py::buffer_protocol(), doc.str().c_str());

  vec_cls.def(py::init<>(), "Initializes all values to 0.");


  std::ostringstream().swap(doc);
  doc << "Creates a vector where each coordinate is set to the given scalar."
         "\n\n**Corresponding C++ API:** ``viren2d::"
      << VC::TypeName() << "::All``.";
  vec_cls.def_static(
        "all",
        &VC::All,
        doc.str().c_str(),
        py::arg("value"));


  std::ostringstream().swap(doc);
  doc << "Creates a vector from a " << dim << "-element :class:`tuple`.";
  vec_cls.def(
        py::init<>(&VecFromTuple<_Tp, dim>), doc.str().c_str(),
        py::arg("tpl"));

  std::ostringstream().swap(doc);
  doc << "Creates a vector from a " << dim << "-element :class:`list`.";
  vec_cls.def(
        py::init<>(&VecFromList<_Tp, dim>), doc.str().c_str(),
        py::arg("lst"));

  std::ostringstream().swap(doc);
  doc << "Creates a vector from a " << dim << "-element :class:`numpy.array`.";
  vec_cls.def(
        py::init<>(&VecFromArray<_Tp, dim>), doc.str().c_str(),
        py::arg("arr"))
      .def_buffer(&VecInfo<_Tp, dim>);


  vec_cls.def(
        "__repr__",
        [](const VC &v) {
          return "<"  + v.ToString(true) + ">";
        });

  vec_cls.def(
        "__str__",
        [](const VC &v) {
          return v.ToString(false);
        });


  std::ostringstream().swap(doc);
  doc << PyTypeName<_Tp>(false)
      << " Read-write access to the first dimension, *i.e.* same as ``vec[0]``.";
  vec_cls.def_property(
        "x",
        static_cast<const _Tp &(VC::*)() const>(&VC::x),
        &VC::SetX,
        doc.str().c_str());


  std::ostringstream().swap(doc);
  doc << PyTypeName<_Tp>(false)
      << " Read-write access to the second dimension, *i.e.* same as ``vec[1]``.";
  vec_cls.def_property(
        "y",
        static_cast<const _Tp &(VC::*)() const>(&VC::y),
        &VC::SetY,
        doc.str().c_str());


  std::ostringstream().swap(doc);
  doc << "int: Number of dimensions (read-only), *i.e.* " << dim << ".";
  vec_cls.def_property_readonly(
        "ndim",
        [](const VC&) { return dim; },
        doc.str().c_str())
      .def(
        "__setitem__",
        [](VC &self, int index, _Tp v) { self[index] = v; },
        "Allows accessing this vector's values via ``vec[idx]``.")
      .def(
        "__getitem__",
        [](const VC &self, int index) -> _Tp { return self[index]; },
        "Allows accessing this vector's values via ``vec[i]``.");

  std::ostringstream().swap(doc);
  doc << "Returns a deep copy.\n\n"
         "**Corresponding C++ API:** Copy constructor of ``viren2d::``."
      << VC::TypeName() << "``.";
  vec_cls.def(
        "copy",
        [](const VC &self) { return VC(self); },
        doc.str().c_str());


  std::ostringstream().swap(doc);
  doc << "Returns :math:`\\max(v_i)`.\n\n**Corresponding C++ API:** ``viren2d::"
      << VC::TypeName() << "::MaxValue``.";
  vec_cls.def("max_value", &VC::MaxValue, doc.str().c_str());


  std::ostringstream().swap(doc);
  doc << "Returns :math:`\\min(v_i)`.\n\n**Corresponding C++ API:** ``viren2d::"
      << VC::TypeName() << "::MinValue``.";
  vec_cls.def("min_value", &VC::MinValue, doc.str().c_str());


  std::ostringstream().swap(doc);
  doc << "Returns :math:`i = \\arg_i \\max(v_i)`.\n\n**Corresponding C++ API:** ``viren2d::"
      << VC::TypeName() << "::MaxIndex``.";
  vec_cls.def("max_index", &VC::MaxIndex, doc.str().c_str());


  std::ostringstream().swap(doc);
  doc << "Returns :math:`i = \\arg_i \\min(v_i)`.\n\n**Corresponding C++ API:** ``viren2d::"
      << VC::TypeName() << "::MinIndex``.";
  vec_cls.def("min_index", &VC::MinIndex, doc.str().c_str());


  std::ostringstream().swap(doc);
  doc << "Returns the length of this vector.\n\n**Corresponding C++ API:** ``viren2d::"
      << VC::TypeName() << "::Length``.";
  vec_cls.def("length", &VC::Length, doc.str().c_str());


  std::ostringstream().swap(doc);
  doc << "Returns the squared length of this vector.\n\n**Corresponding C++ API:** ``viren2d::"
      << VC::TypeName() << "::LengthSquared``.";
  vec_cls.def("length_squared", &VC::LengthSquared, "Returns this vector's squared length.");


  std::ostringstream().swap(doc);
  doc << "Returns the dot product of ``self`` and the other :class:`~"
      << FullyQualifiedType(VC::TypeName()) << "`.";
  vec_cls.def("dot", &VC::Dot, doc.str().c_str(), py::arg("other"));


  std::ostringstream().swap(doc);
  doc << "Returns the Euclidean distance between ``self`` and the other :class:`~"
      << FullyQualifiedType(VC::TypeName()) << "`.";
  vec_cls.def("distance", &VC::Distance, doc.str().c_str(),
           py::arg("other"));


  std::ostringstream().swap(doc);
  doc << "Returns the :class:`~" << FullyQualifiedType(VC::TypeName())
      << "` direction vector, ``other - self``.";
  vec_cls.def("direction_vector", &VC::DirectionVector,
           doc.str().c_str(), py::arg("other"));


  std::ostringstream().swap(doc);
  doc << "Returns the corresponding :class:`~"
      << FullyQualifiedType(VC::TypeName()) << "` unit vector.";
  vec_cls.def("unit_vector", &VC::UnitVector, doc.str().c_str());


  std::ostringstream().swap(doc);
  doc << ":class:`~" << FullyQualifiedType(VC::TypeName())
      << "` instances can be pickled.";
  vec_cls.def(py::pickle(&VecToList<_Tp, dim>, &VecFromList<_Tp, dim>),
                doc.str().c_str());


  vec_cls.def(py::self == py::self, "Checks for equality.")
      .def(py::self != py::self, "Checks for inequality.");


  std::ostringstream().swap(doc);
  doc << "Operator ``vec + vec``.\n\nReturns a :class:`~"
      << FullyQualifiedType(VC::TypeName())
      << "` which is the result of adding the two vectors.";
  vec_cls.def(py::self + py::self, doc.str().c_str());


  std::ostringstream().swap(doc);
  doc << "Operator ``+= vec``.\n\nAdds the other :class:`~"
      << FullyQualifiedType(VC::TypeName())
      << "` values to this vector.";
  vec_cls.def(py::self += py::self, doc.str().c_str(), py::arg("other"));


  std::ostringstream().swap(doc);
  doc << "Operator ``vec - vec``.\n\nReturns a :class:`~"
      << FullyQualifiedType(VC::TypeName())
      << "` which is the result of\n"
      << "subtracting the right-hand side vector from the left-hand side vector.";
  vec_cls.def(py::self - py::self, doc.str().c_str());


  std::ostringstream().swap(doc);
  doc << "Returns a :class:`~" << FullyQualifiedType(VC::TypeName())
      << "`, where all values are negated.";
  vec_cls.def(-py::self, doc.str().c_str());


#ifdef __clang__
// Clang gives false warnings: https://bugs.llvm.org/show_bug.cgi?id=43124
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
#endif  // __clang__
  std::ostringstream().swap(doc);
  doc << "Operator ``-= vec``.\n\nSubtracts the other :class:`~"
      << FullyQualifiedType(VC::TypeName())
      << "` values from this vector.";
  vec_cls.def(py::self -= py::self, doc.str().c_str(), py::arg("other"));
#ifdef __clang__
#pragma GCC diagnostic pop
#endif  // __clang__


  vec_cls.def(py::self * float(),
        "Operator ``vec * float``.\n\n"
        "Returns a vector where all coordinates are multiplied by\n"
        "the :class:`float` scalar.", py::arg("scalar"))
      .def(py::self *= float(),
        "Operator ``*= float``.\n\n"
        "Scales all coordinates by the :class:`float` scalar.",
        py::arg("scalar"))
      .def(float() * py::self,
        "Operator ``float * vec``.\n\n"
        "Returns a vector where all coordinates are multiplied by\n"
        "the :class:`float` scalar.", py::arg("scalar"))
      .def(py::self / float(),
        "Operator ``vec / float``.\n\n"
        "Returns a vector where all coordinates are divided by\n"
        "the :class:`float` scalar.", py::arg("scalar"))
      .def(py::self /= float(),
         "Operator ``vec /= float``.\n\n"
         "Divides all coordinates by the :class:`float` scalar.");


  // Specific for 2-dim vectors
  std::ostringstream().swap(doc);
  doc << "Explicit initialization from " << dim << " separate values.";
  if (dim == 2) {
    vec_cls.def(py::init<_Tp, _Tp>(), py::arg("x"), py::arg("y"), doc.str().c_str());


    std::ostringstream alt;
    alt << PyTypeName<_Tp>(false)
        << " Provides alternative read-write access to\n"
           "  the first dimension, (*i.e.* :attr:`x`).\n\n"
           "  Can be useful if you want to use this 2D vector to\n"
           "  represent a 2D *size*. This property is only available\n"
           "  for :class:`~viren2d.Vec2d` instances.";
    vec_cls.def_property("width", static_cast<const _Tp &(VC::*)() const>(&VC::width),
        &VC::SetWidth, alt.str().c_str());


    std::ostringstream().swap(alt);
    alt << PyTypeName<_Tp>(false)
        << " Provides alternative read-write access to\n"
           "  the second dimension, (*i.e.* :attr:`y`).\n\n"
           "  Can be useful if you want to use this 2D vector to\n"
           "  represent a 2D *size*. This property is only available\n"
           "  for :class:`~viren2d.Vec2d` instances.";
    vec_cls.def_property("height", static_cast<const _Tp &(VC::*)() const>(&VC::height),
                      &VC::SetHeight, alt.str().c_str());
  }

  // Specific for 3-dim vectors
  if (dim == 3) {
    vec_cls.def(
          py::init<_Tp, _Tp, _Tp>(),
          doc.str().c_str(),
          py::arg("x"), py::arg("y"), py::arg("z"));

    vec_cls.def(
          "cross", &VC::Cross,
          "Computes the cross product.",
          py::arg("other"));
  }


  if (dim > 2) {
    std::ostringstream().swap(doc);
    doc << PyTypeName<_Tp>(false)
        << " Read-write access to the third dimension, *i.e.* same as ``vec[2]``.";
    vec_cls.def_property(
          "z",
          static_cast<const _Tp &(VC::*)() const>(&VC::z),
          &VC::SetZ,
          doc.str().c_str());
  }


  // Specific for >=4-dim vectors
  if (dim == 4) {
    vec_cls.def(
          py::init<_Tp, _Tp, _Tp, _Tp>(),
          py::arg("x"), py::arg("y"), py::arg("z"), py::arg("w"));
  }

  if (dim > 3) {
    std::ostringstream().swap(doc);
    doc << PyTypeName<_Tp>(false)
        << " Read-write access to the fourth dimension, *i.e.*  same as ``vec[3]``.";
    vec_cls.def_property(
          "w",
          static_cast<const _Tp &(VC::*)() const>(&VC::w),
          &VC::SetW,
          doc.str().c_str());
  }

  py::implicitly_convertible<py::tuple, VC>();
  py::implicitly_convertible<py::list, VC>();
  py::implicitly_convertible<py::array, VC>();
}


void RegisterVectors(pybind11::module &m) {
  RegisterVec<double, 2>(m);
  RegisterVec<double, 3>(m);

  RegisterVec<int, 2>(m);
  RegisterVec<int, 3>(m);
}

} // namespace bindings
} // namespace viren2d

