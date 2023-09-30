// MIT License

// Copyright (c) 2023 Marc Hartung

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <filesystem>
#include <string>
#include <vector>

#include "utils/types.hpp"
#include "vtk_helper.hpp"

class VTP {
 public:
  VTP() = default;

  void SetPoints(const std::vector<Vectord>& field);

  void SetMesh(const std::vector<Vectord>& points,
               const std::vector<Array<uint32_t, 3>>& segments);

  template <typename T>
  void AddPointData(const std::string& field_name, const T* field,
                    const size_t n) {
    polydata_->GetPointData()->AddArray(
        VtkHelper::ToVtk<T>(field_name, field, n));
  }

  template <typename T>
  void AddSegmentData(const std::string& field_name, const T* field,
                      const size_t n) {
    polydata_->GetCellData()->AddArray(VtkHelper::ToVtk(field_name, field, n));
  }

  void Export(const std::filesystem::path& file_path);

  std::vector<Vectord> GetPoints();
  std::tuple<std::vector<Vectord>, std::vector<Array<uint32_t, 3>>> GetMesh();

  template <typename T>
  std::vector<T> GetPointData(const std::string& field_name) {
    return VtkHelper::FromVtk<T>(
        polydata_->GetPointData()->GetAbstractArray(field_name.c_str()));
  }

  template <typename T>
  std::vector<T> GetSegmentData(const std::string& field_name) {
    return VtkHelper::FromVtk<T>(
        polydata_->GetCellData()->GetAbstractArray(field_name.c_str()));
  }
  void Import(const std::filesystem::path& file_path);

 private:
  vtkSmartPointer<vtkPolyData> polydata_ = vtkSmartPointer<vtkPolyData>::New();
};
