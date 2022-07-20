#include <sstream>
#include <stdexcept>

#include <pybind11/operators.h>

#include <bindings/binding_helpers.h>
#include <werkzeugkiste/geometry/vector.h>


namespace py = pybind11;

namespace viren2d {
namespace bindings {

//------------------------------------------------- Vec(tor) from tuple
template<typename _Tp, int dim>
werkzeugkiste::geometry::Vec<_Tp, dim> VecFromTupleOrList(const py::object &object) {
  const std::string type = py::cast<std::string>(object.attr("__class__").attr("__name__"));
  using VC = werkzeugkiste::geometry::Vec<_Tp, dim>;

  if ((type.compare("tuple") != 0)
      && (type.compare("list") != 0)) {
    std::ostringstream s;
    s << "Cannot cast `" << type << "` to `viren2d." << VC::TypeName()
      << "`, we only support casting from tuples and lists.";
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


template<typename _Tp, int dim>
werkzeugkiste::geometry::Vec<_Tp, dim> VecFromList(py::list lst) {
  return VecFromTupleOrList<_Tp, dim>(lst.cast<py::tuple>());
}



// To support pickling, it is easier to serialize the vector
// as a list. The other option would be to have explicit specializations
// for each type and dimensionality, so we can create a py::tuple via
// py::make_tuple...
template<typename _Tp, int dim>
py::list VecToList(const werkzeugkiste::geometry::Vec<_Tp, dim> &vec) {
  py::list lst;
  for (int i = 0; i < dim; ++i)
    lst.append(vec.val[i]);
  return lst;
}



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



template<typename _Tp, int dim>
void RegisterVec(py::module &m) {
  using VC = werkzeugkiste::geometry::Vec<_Tp, dim>;
  std::ostringstream doc;
  doc << "A " << dim
      << "D vector of " << PyTypeName<_Tp>(true) << " coordinates.";
  py::class_<VC> vec_cls(m, VC::TypeName().c_str(), doc.str().c_str());

  vec_cls.def(py::init<>(), "Initializes all values to 0.");

  std::ostringstream().swap(doc);
  doc << "Creates a vector from a " << dim << "-element :class:`tuple`.";
  vec_cls.def(py::init<>(&VecFromTupleOrList<_Tp, dim>), doc.str().c_str());


  vec_cls.def(
        "__repr__",
        [](const VC &v) {
          return "<"  + VC::TypeName() + v.ToString() + ">";
        });

  vec_cls.def("__str__", &VC::ToString);


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
        "Allows accessing this vector's values via ``vec[i]``.")
      .def(
        "copy",
        [](const VC &self) { return VC(self); },
        "Returns a deep copy.");


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
}


void RegisterVectors(pybind11::module &m) {
  RegisterVec<double, 2>(m);
  RegisterVec<double, 3>(m);

//  RegisterVec<int, 2>(m);
//  RegisterVec<int, 3>(m);
}

} // namespace bindings
} // namespace viren2d

