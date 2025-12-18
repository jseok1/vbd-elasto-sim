import argparse

import pymesh


def tetrahedalize(input_path: str, output_path: str, cell_size: float) -> None:
  input_mesh = pymesh.load_mesh(input_path)

  output_mesh = pymesh.tetrahedralize(input_mesh, cell_size)

  pymesh.save_mesh(output_path, output_mesh, ascii=True)

  # with open(args.output, mode="w") as f:
  #     f.write("$MeshFormat\n4 0 8\n$EndMeshFormat\n")
  #     f.write("$Entities\n0 0 0 1\n")
  #     f.write(
  #         "0 {:g} {:g} {:g} {:g} {:g} {:g} 0 0\n".format(
  #             *output_mesh.nodes.min(axis=0), *output_mesh.nodes.max(axis=0)
  #         )
  #     )
  #     f.write("$EndEntities\n")
  #     f.write("$Nodes\n")
  #     f.write("1 {0:d}\n0 3 0 {0:d}\n".format(output_mesh.num_nodes))
  #     for i, node in enumerate(output_mesh.nodes):
  #         f.write("{:d} {:g} {:g} {:g}\n".format(i + 1, *node))
  #     f.write("$EndNodes\n")
  #
  #     f.write("$Elements\n")
  #     f.write("1 {0:d}\n0 3 4 {0:d}\n".format(output_mesh.num_elements))
  #     f.write("\n")
  #     for i, element in enumerate(output_mesh.elements):
  #         f.write("{:d} {:d} {:d} {:d} {:d}\n".format(i + 1, *(element + 1)))
  #     f.write("$EndElements\n")
  #
  #     f.write("$Surface\n")
  #     f.write("{:d}\n".format(output_mesh.num_faces))
  #     for face in output_mesh.faces:
  #         f.write("{:d} {:d} {:d}\n".format(*(face + 1)))
  #     f.write("$EndSurface\n")


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
    default=4.0,
    help="maximum radius of the circumscribed spheres of the tetrahedrons",
  )
  args = parser.parse_args()

  args.output = args.output_path or args.input.with_suffix(".msh")

  tetrahedalize(args.input_path, args.output_path, args.cell_size)
