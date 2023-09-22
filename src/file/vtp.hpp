#include "utils/types.hpp"
#include "vtk_helper.hpp"
#include <filesystem>
#include <string>
#include <vector>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

class VTP
{
public:
  VTP() = default;

  void SetPoints(const std::vector<Pointd>& field);

  void SetMesh(const std::vector<Pointd>& points,
               const std::vector<Array<uint32_t, 3>>& segments);

  template<typename T>
  void AddPointData(const std::string& field_name,
                    const T* field, const size_t n)
  {
    polydata_->GetPointData()->AddArray(VtkHelper::ToVtk<T>(field_name, field, n));
  }

  template<typename T>
  void AddSegmentData(const std::string& field_name,
                      const T* field, const size_t n)
  {
    polydata_->GetCellData()->AddArray(VtkHelper::ToVtk(field_name, field, n));
  }

  void Export(const std::filesystem::path& file_path);

  std::vector<Pointd> GetPoints();
  std::tuple<std::vector<Pointd>, std::vector<Array<uint32_t, 3>>> GetMesh();

  template<typename T>
  std::vector<T> GetPointData(const std::string& field_name)
  {
    return VtkHelper::FromVtk<T>(
      polydata_->GetPointData()->GetAbstractArray(field_name.c_str()));
  }

  template<typename T>
  std::vector<T> GetSegmentData(const std::string& field_name)
  {
    return VtkHelper::FromVtk<T>(
      polydata_->GetCellData()->GetAbstractArray(field_name.c_str()));
  }
  void Import(const std::filesystem::path& file_path);

private:
  vtkSmartPointer<vtkPolyData> polydata_ = vtkSmartPointer<vtkPolyData>::New();
};
