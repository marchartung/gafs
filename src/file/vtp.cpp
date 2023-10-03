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

#include "vtp.hpp"

#include <vtkArray.h>
#include <vtkCell.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkTriangle.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

void VTP::SetPoints(const std::vector<Vectord>& field) {
  auto vtk_points = vtkSmartPointer<vtkPoints>::New();
  vtk_points->SetDataTypeToDouble();
  auto vtk_vertices = vtkSmartPointer<vtkCellArray>::New();
  vtkIdType pid[1];
  for (size_t i = 0; i != field.size(); i++) {
    pid[0] = vtk_points->InsertNextPoint(field[i][0], field[i][1], field[i][2]);
    vtk_vertices->InsertNextCell(1, pid);
  }
  polydata_->SetPoints(vtk_points);
  polydata_->SetVerts(vtk_vertices);
}

void VTP::SetMesh(const std::vector<Vectord>& points,
                  const std::vector<Array<uint32_t, 3>>& segments) {
  auto vtksegments = vtkSmartPointer<vtkCellArray>::New();
  auto vtkpoints = vtkSmartPointer<vtkPoints>::New();
  for (const auto& point_i : points)
    vtkpoints->InsertNextPoint(point_i[0], point_i[1], point_i[2]);

  vtkSmartPointer<vtkCell> triangle = vtkSmartPointer<vtkTriangle>::New();
  for (auto& segment : segments) {
    triangle->GetPointIds()->SetId(0, segment[0]);
    triangle->GetPointIds()->SetId(1, segment[1]);
    triangle->GetPointIds()->SetId(2, segment[2]);
    vtksegments->InsertNextCell(triangle);
  }
  polydata_->SetPoints(vtkpoints);
  polydata_->SetPolys(vtksegments);
}

void VTP::Export(const std::filesystem::path& file_path) {
  auto writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName(file_path.string().c_str());
  writer->SetInputData(polydata_);
  writer->Write();
}

std::vector<Vectord> VTP::GetPoints() {
  std::vector<Vectord> points;
  auto vtk_points = polydata_->GetPoints();
  points.resize(vtk_points->GetNumberOfPoints());
  for (size_t i = 0; i < points.size(); ++i) {
    vtk_points->GetPoint(i, points[i].data());
  }
  return points;
}

std::tuple<std::vector<Vectord>, std::vector<Array<uint32_t, 3>>>
VTP::GetMesh() {
  vtkPoints* vtkpoints = polydata_->GetPoints();
  std::vector<Vectord> points(vtkpoints->GetNumberOfPoints());
  for (size_t i = 0; i < points.size(); ++i)
    vtkpoints->GetPoint(i, points[i].data());

  std::vector<Array<uint32_t, 3>> segments(polydata_->GetNumberOfCells());
  for (vtkIdType cell_i = 0; cell_i < polydata_->GetNumberOfCells(); cell_i++) {
    auto cell = polydata_->GetCell(cell_i);
    segments[cell_i][0] = cell->GetPointId(0);
    segments[cell_i][1] = cell->GetPointId(1);
    segments[cell_i][2] = cell->GetPointId(2);
  }
  return std::make_tuple(std::move(points), std::move(segments));
}

void VTP::Import(const std::filesystem::path& file_path) {
  auto reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
  reader->SetFileName(file_path.string().c_str());
  vtkObject::GlobalWarningDisplayOff();
  reader->Update();
  vtkObject::GlobalWarningDisplayOn();
  polydata_ = reader->GetOutput();
}
