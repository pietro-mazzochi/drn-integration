from graphviz import Digraph

# Criação do fluxograma detalhado
fluxograma = Digraph(comment='Fluxograma Interno Detalhado do SLAM Toolbox')

# Nós principais detalhados
fluxograma.node('A', '<Início: <i>SLAM Toolbox Node</i>>')
fluxograma.node('B', '<Recepção de Dados (<i>LiDAR</i>/Odometria)>')
fluxograma.node('C', '<Filtragem e Pré-processamento>')
fluxograma.node('D', '<Transformações TF (<i>base_link -&gt; odom)</i>>')
fluxograma.node('E', '<Estimativa de Posição (<i>Scan Matching</i>)>')
fluxograma.node('F', '<Construção de Mapa Incremental (Grade de Ocupação)>')
fluxograma.node('G', '<Detecção de <i>Loop Closure</i>>')
fluxograma.node('H', '<Otimização Global (<i>Graph SLAM</i>)>')
fluxograma.node('I', '<Atualização Contínua do Mapa>')
fluxograma.node('J', '<Publicação do Mapa>')

# Conexões entre os nós
fluxograma.edge('A', 'B', label='< Iniciar Sistema>')
fluxograma.edge('B', 'C', label='< Captura de Dados>')
fluxograma.edge('C', 'D', label='< Filtragem/<i>Downsampling</i>>')
fluxograma.edge('D', 'E', label='< Aplicação de TF>')
fluxograma.edge('E', 'F', label='< Estimativa de Posição>')
fluxograma.edge('F', 'G', label='< Construção de Mapa>')
fluxograma.edge('G', 'H', label='< Detecção de <i>Loops</i>>')
fluxograma.edge('H', 'I', label='< Otimização Global>')
fluxograma.edge('I', 'J', label='< Atualização em Tempo Real>')

# Renderizar o fluxograma em um arquivo
fluxograma.render('fluxograma_detalhado_slam_toolbox', view=True)
