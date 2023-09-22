#pragma once

#include <string>
#include <vtkAbstractArray.h>
#include <vtkSmartPointer.h>

class VtkHelper
{
public:
  template<typename T>
  static vtkSmartPointer<vtkAbstractArray> ToVtk(
    const std::string& field_name,
    const T* field, const size_t n);

  template<typename T>
  static std::vector<T> FromVtk(vtkAbstractArray* array);
};