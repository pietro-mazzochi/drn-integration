import numpy as np
import cv2
import open3d as o3d

# Carregar o arquivo PGM
imagem_pgm = cv2.imread('../../../maps/mapa_isi_rev005.pgm', cv2.IMREAD_GRAYSCALE)

if imagem_pgm is None:
    print("Erro: não foi possível carregar o arquivo PGM. Verifique o caminho do arquivo e a integridade.")
    exit(1)

# Carregar a segunda imagem groundTruth.png

imagem_ground_truth = cv2.imread('../../../maps/ground-truth-1m.png', cv2.IMREAD_GRAYSCALE)

if imagem_ground_truth is None:
    print("Erro: não foi possível carregar a imagem ground-truth-1m.png. Verifique o caminho do arquivo e a integridade.")
    exit(1)

# Obter as dimensões da imagem PGM
altura_pgm, largura_pgm = imagem_pgm.shape

# Redimensionar a imagem groundTruth.png para ter as mesmas dimensões que imagem_pgm
imagem_ground_truth = cv2.resize(imagem_ground_truth, (largura_pgm, altura_pgm), interpolation=cv2.INTER_LINEAR)

# Criar uma lista para armazenar os pontos da primeira imagem (PGM)
pontos_pgm = []
cores_pgm = []

# Gerar pontos e intensidades a partir dos pixels da primeira imagem
for y in range(altura_pgm):
    for x in range(largura_pgm):
        intensidade = imagem_pgm[y, x]
        if(intensidade == 0):
            z = intensidade / 255.0  # Normalizar o valor de intensidade para usar como profundidade
            pontos_pgm.append([x, y, z])
            cores_pgm.append([intensidade / 255.0, intensidade / 255.0, intensidade / 255.0])  # Intensidade como cor
        elif(intensidade > 0 and intensidade < 250):
            z = 0 
            pontos_pgm.append([x, y, z])
            cores_pgm.append([0, 0, 0])  # Intensidade como cor

# Criar uma lista para armazenar os pontos da segunda imagem (groundTruth.png)
pontos_ground_truth = []
cores_ground_truth = []

# Gerar pontos e intensidades a partir dos pixels da segunda imagem
for y in range(altura_pgm):
    for x in range(largura_pgm):
        intensidade = imagem_ground_truth[y, x]
        if(intensidade == 0):
            z = intensidade / 255.0  # Normalizar o valor de intensidade para usar como profundidade
            pontos_ground_truth.append([x, y, z])
            cores_ground_truth.append([intensidade / 255.0, intensidade / 255.0, intensidade / 255.0])  # Intensidade como cor

# Converter as listas de pontos e cores para arrays numpy
nuvem_de_pontos_pgm = np.array(pontos_pgm)
nuvem_de_cores_pgm = np.array(cores_pgm)

nuvem_de_pontos_ground_truth = np.array(pontos_ground_truth)
nuvem_de_cores_ground_truth = np.array(cores_ground_truth)

# Criar objetos PointCloud do Open3D para ambas as imagens
pcl_pgm = o3d.geometry.PointCloud()
pcl_pgm.points = o3d.utility.Vector3dVector(nuvem_de_pontos_pgm)
pcl_pgm.colors = o3d.utility.Vector3dVector(nuvem_de_cores_pgm)

pcl_ground_truth = o3d.geometry.PointCloud()
pcl_ground_truth.points = o3d.utility.Vector3dVector(nuvem_de_pontos_ground_truth)
pcl_ground_truth.colors = o3d.utility.Vector3dVector(nuvem_de_cores_ground_truth)

# Visualizar ambas as nuvens de pontos
o3d.visualization.draw_geometries([pcl_pgm], window_name="Nuvem de Pontos - PGM")
o3d.visualization.draw_geometries([pcl_ground_truth], window_name="Nuvem de Pontos - Ground Truth")

# Salvar ambas as nuvens de pontos em arquivos PLY
o3d.io.write_point_cloud("mapa_isi_rev005.ply", pcl_pgm)
o3d.io.write_point_cloud("nuvem_de_pontos_ground_truth.ply", pcl_ground_truth)

print("Nuvem de pontos PGM salva como 'nuvem_de_pontos_pgm.ply'.")
print("Nuvem de pontos Ground Truth salva como 'nuvem_de_pontos_ground_truth.ply'.")
