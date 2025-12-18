import argparse
import itertools
import json

import numpy as np
import pymesh


def parametrize(path: str, density: float, young: float, poisson: float) -> None:
  mesh = pymesh.load_mesh(path)
  V = mesh.num_vertices
  E = mesh.num_elements

  mu_ = young / (2 * (1 + poisson))
  lambda_ = young * poisson / ((1 + poisson) * (1 - 2 * poisson))

  stiffness = []
  mass = [0] * V

  vertex_to_tetrahedra = [[] for _ in range(V)]

  for e, element in enumerate(mesh.elements):
    v1, v2, v3, v4 = element

    vertices = mesh.vertices[[v1, v2, v3, v4]]
    Dm = np.stack(
      [vertices[1] - vertices[0], vertices[2] - vertices[0], vertices[3] - vertices[0]], axis=1
    )

    if np.linalg.det(Dm) < 0:
      v2, v3 = v3, v2
      vertices = mesh.vertices[[v1, v2, v3, v4]]
      Dm = np.stack(
        [vertices[1] - vertices[0], vertices[2] - vertices[0], vertices[3] - vertices[0]], axis=1
      )

    assert np.linalg.det(Dm) > 0

    vertex_to_tetrahedra[v1].append(e)
    vertex_to_tetrahedra[v2].append(e)
    vertex_to_tetrahedra[v3].append(e)
    vertex_to_tetrahedra[v4].append(e)

    volume = np.linalg.det(Dm) / 6.0

    grads = np.zeros((4, 3))
    grads[1:] = np.linalg.inv(Dm).T
    grads[0] = -np.sum(grads[1:], axis=0)

    B = np.zeros((6, 12))
    for i in range(4):
      gx, gy, gz = grads[i]
      B[:, i * 3 : i * 3 + 3] = [
        [gx, 0, 0],
        [0, gy, 0],
        [0, 0, gz],
        [gy, gx, 0],
        [0, gz, gy],
        [gz, 0, gx],
      ]

    C = np.array(
      [
        [lambda_ + 2 * mu_, lambda_, lambda_, 0, 0, 0],
        [lambda_, lambda_ + 2 * mu_, lambda_, 0, 0, 0],
        [lambda_, lambda_, lambda_ + 2 * mu_, 0, 0, 0],
        [0, 0, 0, mu_, 0, 0],
        [0, 0, 0, 0, mu_, 0],
        [0, 0, 0, 0, 0, mu_],
      ]
    )

    stiffness.extend((volume * B.T @ C @ B).flatten().tolist())

    mass[v1] += density * volume / 4
    mass[v2] += density * volume / 4
    mass[v3] += density * volume / 4
    mass[v4] += density * volume / 4

  # for this to not be an I/O bottleneck, you should arrange neighboring vertices and tetrahedra together in memory
  vertex_to_tetrahedra_offsets = [0] * (V + 1)
  for v in range(1, V + 1):
    vertex_to_tetrahedra_offsets[v] = vertex_to_tetrahedra_offsets[v - 1] + len(
      vertex_to_tetrahedra[v - 1]
    )

  with open("/workspace/assets/experiments/01/vertex-to-tetrahedra.json", "w") as file:
    json.dump(list(itertools.chain.from_iterable(vertex_to_tetrahedra)), file)

  with open("/workspace/assets/experiments/01/vertex-to-tetrahedra-offsets.json", "w") as file:
    json.dump(vertex_to_tetrahedra_offsets, file)

  with open("/workspace/assets/experiments/01/positions.json", "w") as file:
    json.dump(mesh.vertices.flatten().tolist(), file)

  with open("/workspace/assets/experiments/01/tetrahedron-indices.json", "w") as file:
    json.dump(mesh.elements.flatten().tolist(), file)

  with open("/workspace/assets/experiments/01/masses.json", "w") as file:
    json.dump(mass, file)

  with open("/workspace/assets/experiments/01/stiffnesses.json", "w") as file:
    json.dump(stiffness, file)

  # surface triangles
  # faces = np.vstack(
  #   [
  #     mesh.elements[:, [0, 1, 2]],
  #     mesh.elements[:, [0, 1, 3]],
  #     mesh.elements[:, [0, 2, 3]],
  #     mesh.elements[:, [1, 2, 3]],
  #   ]
  # )
  # faces = np.sort(faces, axis=1)
  # face_counts = Counter(map(tuple, faces))
  # surface_faces = np.array([face for face, count in face_counts.items() if count == 1])

  face_owner = {}
  for e, element in enumerate(mesh.elements):
    faces = [
      (element[0], element[1], element[2]),
      (element[0], element[1], element[3]),
      (element[0], element[2], element[3]),
      (element[1], element[2], element[3]),
    ]
    for face in faces:
      face_key = tuple(sorted(face))
      if face_key not in face_owner:
        face_owner[face_key] = e  # potentially exterior face
      else:
        face_owner[face_key] = None  # interior face

  oriented_faces = []
  for face_key, e in face_owner.items():
    if e is None:
      continue

    # unordered face vertices
    i, j, k = face_key
    i, j, k = int(i), int(j), int(k)

    # find opposite vertex in tet
    element = mesh.elements[e]
    opp = next(v for v in element if v not in (i, j, k))

    # positions
    pi, pj, pk = mesh.vertices[i], mesh.vertices[j], mesh.vertices[k]
    popp = mesh.vertices[opp]

    # candidate orientation
    n = np.cross(pj - pi, pk - pi)

    # check if normal points outward
    if np.dot(n, popp - pi) > 0:
      oriented_faces.extend([i, k, j])
    else:
      oriented_faces.extend([i, j, k])

  # normals should point outward (rough check)
  # centroid = mesh.vertices.mean(axis=0)
  # for i, j, k in oriented_faces:
  #   n = np.cross(mesh.vertices[j] - mesh.vertices[i], mesh.vertices[k] - mesh.vertices[i])
  #   assert np.dot(n, mesh.vertices[i] - centroid) > 0

  with open("/workspace/assets/experiments/01/triangle-indices.json", "w") as file:
    json.dump(oriented_faces, file)

  # vertex pre-coloring
  ordering = sorted([v for v in range(V)], key=lambda v: len(vertex_to_tetrahedra[v]))
  colors = [None] * V
  for v in ordering:
    used_colors = set()
    for e in vertex_to_tetrahedra[v]:
      element = mesh.elements[e]
      for u in element:
        # u is a neighbor of v
        if u != v and colors[u] is not None:
          used_colors.add(colors[u])

    color = 0
    while color in used_colors:
      color += 1

    colors[v] = color

  # test
  for element in mesh.elements:
    v1, v2, v3, v4 = element
    assert colors[v1] != colors[v2]
    assert colors[v1] != colors[v3]
    assert colors[v1] != colors[v4]
    assert colors[v2] != colors[v3]
    assert colors[v2] != colors[v4]
    assert colors[v3] != colors[v4]

  C = max(colors) + 1
  color_groups = [[] for _ in range(C)]
  for v, color in enumerate(colors):
    color_groups[color].append(v)

  color_group_offsets = [0] * (C + 1)
  for g in range(1, C + 1):
    color_group_offsets[g] = color_group_offsets[g - 1] + len(color_groups[g - 1])

  with open("/workspace/assets/experiments/01/color-groups.json", "w") as file:
    json.dump(list(itertools.chain.from_iterable(color_groups)), file)

  with open("/workspace/assets/experiments/01/color-group-offsets.json", "w") as file:
    json.dump(color_group_offsets, file)


if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("-i", "--path", type=str, required=True, help="path to tetrahedral mesh")
  parser.add_argument(
    "-d",
    "--density",
    type=float,
    required=False,
    default=1e3,
    help="density (ρ)",
  )
  parser.add_argument(
    "-y",
    "--young",
    type=float,
    required=False,
    default=1e5,
    help="Young's modulus (E)",
  )
  parser.add_argument(
    "-p",
    "--poisson",
    type=float,
    required=False,
    default=0.3,
    help="Poisson ratio (ν)",
  )
  args = parser.parse_args()

  parametrize(args.path, args.density, args.young, args.poisson)
