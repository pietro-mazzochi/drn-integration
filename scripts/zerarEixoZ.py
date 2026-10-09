import open3d as o3d
import numpy as np

# Carrega a nuvem de pontos do arquivo PCD
pcd = o3d.io.read_point_cloud("groundTruthChaoDeFabrica.pcd")

# Converte a nuvem de pontos para um array numpy
points = np.asarray(pcd.points)

# Define o eixo Z (terceira coluna) como zero
points[:, 2] = 0

# Atualiza a nuvem de pontos com os valores Z zerados
pcd.points = o3d.utility.Vector3dVector(points)

# Salva a nuvem de pontos modificada
o3d.io.write_point_cloud("groundTruthIsi_z_zero.pcd", pcd)

# Exibe a nuvem de pontos
o3d.visualization.draw_geometries([pcd], window_name="Nuvem de Pontos Z Zerado")
