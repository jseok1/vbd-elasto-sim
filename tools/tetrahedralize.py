import argparse

import pymesh


def tetrahedalize(input_path: str, output_path: str, cell_size: float) -> None:
  input_mesh = pymesh.load_mesh(input_path)

  (input_mesh, _) = pymesh.remove_duplicated_vertices(input_mesh)
  (input_mesh, _) = pymesh.remove_degenerated_triangles(input_mesh)
  (input_mesh, _) = pymesh.remove_duplicated_faces(input_mesh)
  input_mesh = pymesh.resolve_self_intersection(input_mesh)
  input_mesh = pymesh.compute_outer_hull(input_mesh)

  print("is manifold:", input_mesh.is_manifold())
  print("is closed:", input_mesh.is_closed())

  output_mesh = pymesh.tetrahedralize(input_mesh, cell_size, engine="tetgen")

  pymesh.save_mesh(output_path, output_mesh, ascii=True)


if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("-i", "--input-path", type=str, required=True, help="path to input mesh")
  parser.add_argument(
    "-o",
    "--output-path",
    type=str,
    required=False,
    default=None,
    help="path to output mesh",
  )
  parser.add_argument(
    "-s",
    "--cell-size",
    type=float,
    required=False,
    default=1.0,
    help="maximum radius of the circumscribed spheres of the tetrahedrons",
  )
  args = parser.parse_args()

  args.output = args.output_path or args.input.with_suffix(".msh")

  tetrahedalize(args.input_path, args.output_path, args.cell_size)
