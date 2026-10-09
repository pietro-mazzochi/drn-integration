from graphviz import Digraph

# Criação do fluxograma detalhado
fluxograma_amcl = Digraph(comment='Fluxograma Detalhado do Processo de Localização com AMCL e Ciclo de Vida')

# Primeira parte: Ciclo de vida do nó AMCL
fluxograma_amcl.node('L1', '<<i>Unconfigured</i> (Não Configurado)>')
fluxograma_amcl.node('L2', '<<i>Inactive</i> (Inativo)>')
fluxograma_amcl.node('L3', '<<i>Active</i> (Ativo)>')
fluxograma_amcl.node('L4', '<Finalização>')

# Conexões do ciclo de vida
fluxograma_amcl.edge('L1', 'L2', label=' Configurar')
fluxograma_amcl.edge('L2', 'L3', label=' Ativar')
fluxograma_amcl.edge('L3', 'L4', label=' Finalizar')

# Primeira parte: Inicialização e Recepção de Dados
fluxograma_amcl.node('A', '<Início: <i>AMCL Node</i>>')
fluxograma_amcl.node('B', '<Recepção de Dados (<i>LiDAR</i>/Odometria)>')
fluxograma_amcl.node('C', 'Inicialização da Nuvem de Partículas')

# Conexões da primeira parte
fluxograma_amcl.edge('L3', 'A', label=' Iniciar AMCL')  # Integrando com o ciclo de vida
fluxograma_amcl.edge('A', 'B', label=' Receber Dados')
fluxograma_amcl.edge('B', 'C', label=' Iniciar Nuvem de Partículas')

# Segunda parte: Previsão, Atualização e Resampling (horizontal)
with fluxograma_amcl.subgraph() as s:
    s.attr(rankdir='LR')  # Configura o layout horizontal (da esquerda para a direita)
    s.node('D', 'Previsão de Movimento (Modelagem de Odometria)')
    s.node('E', '<Atualização Sensorial (<i>LiDAR</i>)>')
    s.node('F', '<<i>Resampling</i> das Partículas>')

    s.edge('C', 'D', label=' Prever Movimento')
    s.edge('D', 'E', label=' Comparar Dados de Sensores')
    s.edge('E', 'F', label=' Ajustar Nuvem de Partículas')

# Terceira parte: Estimativa e Finalização (vertical)
fluxograma_amcl.node('G', 'Estimativa da Posição')
fluxograma_amcl.node('H', 'Publicação da Posição')
fluxograma_amcl.node('I', 'Atualização Contínua')
fluxograma_amcl.node('J', 'Finalização')

# Conexões da terceira parte
fluxograma_amcl.edge('F', 'G', label=' Calcular Posição Estimada')
fluxograma_amcl.edge('G', 'H', label=' Publicar Posição Estimada')
fluxograma_amcl.edge('H', 'I', label=' Atualização Contínua')
fluxograma_amcl.edge('I', 'J', label=' Finalizar')
fluxograma_amcl.edge('J', 'L4', label=' Encerrar')  # Retorno ao ciclo de vida final

# Renderizar o fluxograma em um arquivo
fluxograma_amcl.render('fluxograma_amcl_ciclo_de_vida', view=True)

