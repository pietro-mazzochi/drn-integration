import open3d as o3d
import numpy as np

# Função para obter o tamanho da nuvem de pontos (dimensões)
def get_cloud_dimensions(pcd):
    points = np.asarray(pcd.points)
    min_bound = points.min(axis=0)
    max_bound = points.max(axis=0)
    return max_bound - min_bound

# Carrega a primeira nuvem de pontos em formato PLY
pcd1 = o3d.io.read_point_cloud("mapa_isi_rev004.ply")
points1 = np.asarray(pcd1.points)

# Carrega a segunda nuvem de pontos em formato PCD
pcd2 = o3d.io.read_point_cloud("groundTruthIsi_z_zero.pcd")
points2 = np.asarray(pcd2.points)

# Obtém as dimensões da primeira e da segunda nuvem
dim1 = get_cloud_dimensions(pcd1)
dim2 = get_cloud_dimensions(pcd2)

# Exibe as dimensões das nuvens
print(f"dim1: {dim1}")
print(f"dim2: {dim2}")

# Calcula o fator de escala apenas para os eixos X e Y (ignorando o Z que já é zero)
scale_factor = dim1[:2] / dim2[:2]

# Aplica o fator de escala (somente nos eixos X e Y) à segunda nuvem de pontos
points2_scaled = points2.copy()
points2_scaled[:, 0:2] *= scale_factor  # Escala os eixos X e Y

# Atualiza a segunda nuvem de pontos com os novos pontos escalados
pcd2.points = o3d.utility.Vector3dVector(points2_scaled)

# Salva a segunda nuvem de pontos escalada
o3d.io.write_point_cloud("groundTruthIsi_z_zero_escalada2-005cm.pcd", pcd2)

# Exibe as duas nuvens de pontos para comparação
o3d.visualization.draw_geometries([pcd1, pcd2], window_name="Nuvens de Pontos - PLY e PCD")
