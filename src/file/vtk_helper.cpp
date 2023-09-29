
#include "vtk_helper.hpp"
#include "utils/types.hpp"
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkSmartPointer.h>
#include <vtkTypeUInt32Array.h>

template<>
vtkSmartPointer<vtkAbstractArray>
VtkHelper::ToVtk<double>(const std::string& field_name,
                         const double* field, const size_t n)
{
  auto array_tmp = vtkSmartPointer<vtkDoubleArray>::New();
  array_tmp->SetName(field_name.c_str());
  array_tmp->SetNumberOfComponents(1);
  for (size_t i = 0; i != n; i++) {
    array_tmp->InsertNextTuple1(field[i]);
  }
  return array_tmp;
}

template<>
vtkSmartPointer<vtkAbstractArray>
VtkHelper::ToVtk<float>(const std::string& field_name,
                        const float* field, const size_t n)
{
  auto array_tmp = vtkSmartPointer<vtkFloatArray>::New();
  array_tmp->SetName(field_name.c_str());
  array_tmp->SetNumberOfComponents(1);
  for (size_t i = 0; i != n; i++) {
    array_tmp->InsertNextTuple1(field[i]);
  }
  return array_tmp;
}

template<>
vtkSmartPointer<vtkAbstractArray>
VtkHelper::ToVtk<uint32_t>(const std::string& field_name,
                           const uint32_t* field, const size_t n)
{
  auto array_tmp = vtkSmartPointer<vtkTypeUInt32Array>::New();
  array_tmp->SetName(field_name.c_str());
  array_tmp->SetNumberOfComponents(1);
  for (size_t i = 0; i != n; i++) {
    array_tmp->InsertNextTuple1(field[i]);
  }
  return array_tmp;
}

template<>
vtkSmartPointer<vtkAbstractArray>
VtkHelper::ToVtk<Vectord>(const std::string& field_name,
                         const Vectord* field, const size_t n)
{
  auto array_tmp = vtkSmartPointer<vtkDoubleArray>::New();
  array_tmp->SetName(field_name.c_str());
  array_tmp->SetNumberOfComponents(3);
  for (size_t i = 0; i != n; i++) {
    array_tmp->InsertNextTuple3(field[i][0], field[i][1], field[i][2]);
  }
  return array_tmp;
}
template<>
vtkSmartPointer<vtkAbstractArray>
VtkHelper::ToVtk<Vectorf>(const std::string& field_name,
                         const Vectorf* field, const size_t n)
{
  auto array_tmp = vtkSmartPointer<vtkFloatArray>::New();
  array_tmp->SetName(field_name.c_str());
  array_tmp->SetNumberOfComponents(3);
  for (size_t i = 0; i != n; i++) {
    array_tmp->InsertNextTuple3(field[i][0], field[i][1], field[i][2]);
  }
  return array_tmp;
}

template<>
std::vector<double>
VtkHelper::FromVtk<double>(vtkAbstractArray* in_array)
{
  const size_t num_values = in_array->GetNumberOfValues();
  vtkSmartPointer<vtkDoubleArray> array =
    vtkDoubleArray::SafeDownCast(in_array);
  std::vector<double> res(num_values);
  for (size_t i = 0; i < num_values; ++i) {
    res[i] = array->GetValue(i);
  }
  return res;
}

template<>
std::vector<float>
VtkHelper::FromVtk<float>(vtkAbstractArray* in_array)
{
  const size_t num_values = in_array->GetNumberOfValues();
  vtkSmartPointer<vtkFloatArray> array = vtkFloatArray::SafeDownCast(in_array);
  std::vector<float> res(num_values);
  for (size_t i = 0; i < num_values; ++i) {
    res[i] = array->GetValue(i);
  }
  return res;
}

template<>
std::vector<uint32_t>
VtkHelper::FromVtk<uint32_t>(vtkAbstractArray* in_array)
{
  const size_t num_values = in_array->GetNumberOfValues();
  vtkSmartPointer<vtkTypeUInt32Array> array =
    vtkTypeUInt32Array::SafeDownCast(in_array);
  std::vector<uint32_t> res(num_values);
  for (size_t i = 0; i < num_values; ++i) {
    res[i] = array->GetValue(i);
  }
  return res;
}

template<>
std::vector<Vectord>
VtkHelper::FromVtk<Vectord>(vtkAbstractArray* in_array)
{
  const size_t num_values = in_array->GetNumberOfTuples();
  vtkSmartPointer<vtkDoubleArray> array =
    vtkDoubleArray::SafeDownCast(in_array);
  std::vector<Vectord> res(num_values);
  for (size_t i = 0; i < num_values; ++i) {
    res[i] = Vectord{
      { array->GetTuple3(i)[0], array->GetTuple3(i)[1], array->GetTuple3(i)[2] }
    };
  }
  return res;
}

template<>
std::vector<Vectorf>
VtkHelper::FromVtk<Vectorf>(vtkAbstractArray* in_array)
{
  const size_t num_values = in_array->GetNumberOfTuples();
  vtkSmartPointer<vtkFloatArray> array = vtkFloatArray::SafeDownCast(in_array);
  std::vector<Vectorf> res(num_values);
  for (size_t i = 0; i < num_values; ++i) {
    res[i] = Vectorf{ { static_cast<float>(array->GetTuple3(i)[0]),
                       static_cast<float>(array->GetTuple3(i)[1]),
                       static_cast<float>(array->GetTuple3(i)[2]) } };
  }
  return res;
}
