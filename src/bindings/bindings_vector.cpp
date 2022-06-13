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
werkzeugkiste::geometry::Vec<_Tp, dim> VecFromTupleOrList(py::object object) {
  const std::string type = py::cast<std::string>(object.attr("__class__").attr("__name__"));
  using VC = werkzeugkiste::geometry::Vec<_Tp, dim>;

  if (type.compare("tuple") != 0 && type.compare("list") != 0) {
    std::ostringstream s;
    s << "Cannot cast " << type << " to " << FullyQualifiedType(VC::TypeName())
      << ", we only support casting from tuples and lists.";
    throw std::invalid_argument(s.str());
  }

  py::tuple tpl = object.cast<py::tuple>();
  if (!tpl.empty() && tpl.size() != dim) {
    std::ostringstream s;
    s << "Cannot create " << FullyQualifiedType(VC::TypeName())
      << " from " << tpl.size() << " values, expected "
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


template<typename _Tp, int dim>
py::list VecToList(const werkzeugkiste::geometry::Vec<_Tp, dim> &vec) {
  py::list lst;
  for (int i = 0; i < dim; ++i)
    lst.append(vec.val[i]);
  return lst;
}


template<typename _Tp, int dim>
void RegisterVec(py::module &m) {
  using VC = werkzeugkiste::geometry::Vec<_Tp, dim>;
  std::ostringstream doc;
  doc << "A " << dim << "D vector.";

  py::class_<VC> vec_cls(m, VC::TypeName().c_str(), doc.str().c_str());

  std::ostringstream doc_tpl;
  doc_tpl << "A " << dim << "-element ``tuple`` can be cast into a :class:`"
          << FullyQualifiedType(VC::TypeName()) << "`.";

  vec_cls.def(py::init<>(), "Initializes all dimensions to 0.")
      .def(py::init<>(&VecFromTupleOrList<_Tp, dim>),
           doc_tpl.str().c_str())
      .def("__repr__",
           [](const VC &v)
           { return FullyQualifiedType(v.ToString(), true); })
      .def("__str__", &VC::ToString)
      .def_property("x", static_cast<const _Tp &(VC::*)() const>(&VC::x), &VC::SetX,
                    "Accesses the first dimension.")
      .def_property("y", static_cast<const _Tp &(VC::*)() const>(&VC::y), &VC::SetY,
                    "Accesses the second dimension.")
      .def_property_readonly("ndim", [](const VC&) { return dim; },
                             "Number of dimensions.")
      .def("__setitem__", [](VC &self, int index, _Tp v)
                          { self[index] = v; })
      .def("__getitem__", [](const VC &self, int index)
                          { return self[index]; })
      .def("copy", [](const VC &self) { return VC(self); },
           "Returns a deep copy.")
      .def("max_value", &VC::MaxValue,
           "Returns ``max(self.val[i])``.")
      .def("min_value", &VC::MinValue,
           "Returns ``min(self.val[i])``.")
      .def("max_index", &VC::MaxIndex,
           "Returns index ``j``, s.t. ``self.val[j] == max(self.val[i])``.")
      .def("min_index", &VC::MinIndex,
           "Returns index ``j``, s.t. ``self.val[j] == min(self.val[i])``.")
      .def("dot", &VC::Dot,
           "Computes the dot product between ``self`` and the ``other``.",
           py::arg("other"))
      .def("length", &VC::Length,
           "Returns the vector's length.")
      .def("length_squared", &VC::LengthSquared,
           "Returns the squared length.")
      .def("distance", &VC::Distance,
           "Computes the L2 distance between ``self`` and the ``other``.",
           py::arg("other"))
      .def("direction_vector", &VC::DirectionVector,
           "Computes the direction vector from ``self`` to the ``other``.",
           py::arg("other"))
      .def("unit_vector", &VC::UnitVector,
           "Computes the unit vector of ``self``.")
      .def(py::pickle(&VecToList<_Tp, dim>,
                      &VecFromList<_Tp, dim>))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def(py::self + py::self)
      .def(py::self += py::self)
      .def(py::self - py::self)
      .def(-py::self)
#ifdef __clang__
// Clang gives false warnings: https://bugs.llvm.org/show_bug.cgi?id=43124
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
#endif  // __clang__
      .def(py::self -= py::self)
#ifdef __clang__
#pragma GCC diagnostic pop
#endif  // __clang__
      .def(py::self * float())
      .def(py::self *= float())
      .def(float() * py::self)
      .def(py::self / float())
      .def(py::self /= float());


  // Specific for 2-dim vectors
  if (dim == 2) {
    vec_cls.def(py::init<_Tp, _Tp>(),
                py::arg("x"), py::arg("y"))
        .def_property("width", static_cast<const _Tp &(VC::*)() const>(&VC::width),
                      &VC::SetWidth,
                      "Accesses the first dimension (if you\n"
                      "use this vector to represent a 2D size).")
        .def_property("height", static_cast<const _Tp &(VC::*)() const>(&VC::height),
                      &VC::SetHeight,
                      "Accesses the second dimension (if you\n"
                      "use this vector to represent a 2D size).");
  }

  // Specific for 3-dim vectors
  if (dim == 3)
  {
    vec_cls.def(py::init<_Tp, _Tp, _Tp>(),
                py::arg("x"), py::arg("y"), py::arg("z"));
    vec_cls.def("cross", &VC::Cross,
                "Computes the cross product.", py::arg("other"));
  }

  if (dim > 2)
    vec_cls.def_property("z", static_cast<const _Tp &(VC::*)() const>(&VC::z),
                         &VC::SetZ, "Accesses the third dimension.");

  // Specific for >=4-dim vectors
  if (dim == 4)
    vec_cls.def(py::init<_Tp, _Tp, _Tp, _Tp>(),
                py::arg("x"), py::arg("y"), py::arg("z"), py::arg("w"));

  if (dim > 3)
    vec_cls.def_property("w", static_cast<const _Tp &(VC::*)() const>(&VC::w),
                         &VC::SetW, "Accesses the fourth dimension.");

  py::implicitly_convertible<py::tuple, VC>();
  py::implicitly_convertible<py::list, VC>();
}


void RegisterVectors(pybind11::module &m) {
  RegisterVec<double, 2>(m);
  RegisterVec<double, 3>(m);
}

} // namespace bindings
} // namespace viren2d

