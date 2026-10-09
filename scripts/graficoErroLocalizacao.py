import csv
import numpy as np
import matplotlib.pyplot as plt

# Função para carregar os dados de um arquivo CSV
def load_data(file_name):
    data = []
    with open(file_name, 'r') as file:
        reader = csv.DictReader(file)
        for row in reader:
            data.append(row)
    return data

# Função para calcular o erro de posição
def calculate_position_error(amcl_data, ground_truth_data):
    errors = []
    for amcl_pose, gt_pose in zip(amcl_data, ground_truth_data):
        amcl_x, amcl_y = float(amcl_pose['x']), float(amcl_pose['y'])
        gt_x, gt_y = float(gt_pose['x']), float(gt_pose['y'])
        
        # Calcula o erro de posição usando a distância euclidiana 2D
        error = np.sqrt((gt_x - amcl_x) ** 2 + (gt_y - amcl_y) ** 2)
        errors.append(error)
    return errors

# Função para plotar as poses
def plot_poses(amcl_data, ground_truth_data):
    plt.figure(figsize=(10, 6))
    
    # Extraindo as posições x e y de AMCL Pose e Ground Truth
    amcl_x = [float(pose['x']) for pose in amcl_data]
    amcl_y = [-float(pose['y']) for pose in amcl_data]  # Invertendo o sinal de Y de AMCL
    
    gt_x = [float(pose['x']) for pose in ground_truth_data]
    gt_y = [-float(pose['y']) for pose in ground_truth_data]  # Invertendo o sinal de Y do Ground Truth
    
    # Invertendo os eixos X e Y durante o plot (X vira Y e Y vira X)
    plt.plot(amcl_y, amcl_x, label='Posição AMCL', color='blue', marker='o')
    plt.plot(gt_y, gt_x, label='Posição Verdadeira', color='green', marker='x')
    
    # Configurações do gráfico para garantir que a escala dos eixos seja igual
    plt.gca().set_aspect('equal', adjustable='box')
    
    # Configurações do gráfico
    plt.title('Posição AMCL vs Posição Verdadeira (X e Y Invertidos, Sinal de Y Invertido)', fontsize=24)
    plt.xlabel('Posição Y (metros)', fontsize=24)
    plt.ylabel('Posição X (metros)', fontsize=24)
    plt.legend(fontsize=24)  # Ajustando o tamanho da legenda
    plt.grid(True)
    
    # Salvar o gráfico como um arquivo e mostrar na tela
    plt.savefig('posicao_amcl_vs_posicao_verdadeira_xy_invertido_sinal_y.png')
    plt.show()

# Função para plotar o erro de posicionamento
def plot_position_error(errors):
    plt.figure(figsize=(10, 6))
    
    # Plotando o erro de posicionamento
    plt.plot(errors, label='Erro de Posicionamento', color='red', linestyle='--')
    
    # Configurações do gráfico
    plt.title('Erro de Posicionamento ao Longo do Tempo', fontsize=24)
    plt.xlabel('Passo de Tempo', fontsize=24)
    plt.ylabel('Erro (metros)', fontsize=24)
    
    # Ajustando os intervalos do eixo Y para ficar de 0.5 em 0.5 metros
    plt.yticks(np.arange(0, max(errors) + 0.5, 0.5))
    
    plt.legend(fontsize=24)  # Ajustando o tamanho da legenda
    plt.grid(True)
    
    # Salvar o gráfico como um arquivo e mostrar na tela
    plt.savefig('erro_de_posicionamento.png')
    plt.show()

# Carregar os dados dos arquivos CSV
amcl_data = load_data('amcl_pose_map_isi_rev005.csv')
ground_truth_data = load_data('ground_truth_map_isi_rev005.csv')

# Certifique-se de que os dados estão sincronizados e com o mesmo número de entradas
min_length = min(len(amcl_data), len(ground_truth_data))
amcl_data = amcl_data[:min_length]
ground_truth_data = ground_truth_data[:min_length]

# Calcular o erro de posicionamento
position_errors = calculate_position_error(amcl_data, ground_truth_data)

# Plotar as poses separadamente e o erro de posicionamento
plot_poses(amcl_data, ground_truth_data)
plot_position_error(position_errors)
