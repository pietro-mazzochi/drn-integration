#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import PoseWithCovarianceStamped
from nav_msgs.msg import Odometry
import csv

class PoseSaver(Node):
    def __init__(self):
        super().__init__('pose_saver_node')
        
        # Inicializando listas para armazenar os dados
        self.amcl_pose_data = []
        self.ground_truth_data = []
        self.latest_ground_truth = None  # Armazenar a última mensagem do /ground_truth
        
        # Subscritores
        self.amcl_pose_sub = self.create_subscription(
            PoseWithCovarianceStamped,
            '/amcl_pose',
            self.amcl_pose_callback,
            10)
        self.ground_truth_sub = self.create_subscription(
            Odometry,
            '/odom', #'/ground_truth',
            self.ground_truth_callback,
            10)
        
    def amcl_pose_callback(self, msg):
        # Salvando posição de /amcl_pose (x, y, z e a orientação quaternion)
        self.amcl_pose_data.append({
            'x': msg.pose.pose.position.x,
            'y': msg.pose.pose.position.y,
            'z': msg.pose.pose.position.z,
            'qx': msg.pose.pose.orientation.x,
            'qy': msg.pose.pose.orientation.y,
            'qz': msg.pose.pose.orientation.z,
            'qw': msg.pose.pose.orientation.w,
            'covariance': msg.pose.covariance
        })
        self.get_logger().info('AMCL Pose received and saved')

        # Apenas salva o ground_truth se a última mensagem estiver disponível
        if self.latest_ground_truth is not None:
            self.ground_truth_data.append(self.latest_ground_truth)
            self.get_logger().info('Ground Truth Pose saved due to AMCL Pose')

    def ground_truth_callback(self, msg):
        # Armazena a última posição de /ground_truth (x, y, z e a orientação quaternion)
        self.latest_ground_truth = {
            'x': msg.pose.pose.position.x,
            'y': msg.pose.pose.position.y,
            'z': msg.pose.pose.position.z,
            'qx': msg.pose.pose.orientation.x,
            'qy': msg.pose.pose.orientation.y,
            'qz': msg.pose.pose.orientation.z,
            'qw': msg.pose.pose.orientation.w
        }
        self.get_logger().info('Ground Truth Pose received and stored for later use')

    def save_to_csv(self):
        # Salvando os dados das poses do amcl_pose em um arquivo CSV
        if self.amcl_pose_data:
            with open('amcl_pose_data.csv', 'w', newline='') as csvfile1:
                writer1 = csv.DictWriter(csvfile1, fieldnames=self.amcl_pose_data[0].keys())
                writer1.writeheader()
                writer1.writerows(self.amcl_pose_data)
            self.get_logger().info('AMCL Pose data saved to CSV file')
        
        # Salvando os dados das poses do ground_truth em um arquivo CSV
        if self.ground_truth_data:
            with open('ground_truth_data.csv', 'w', newline='') as csvfile2:
                writer2 = csv.DictWriter(csvfile2, fieldnames=self.ground_truth_data[0].keys())
                writer2.writeheader()
                writer2.writerows(self.ground_truth_data)
            self.get_logger().info('Ground Truth data saved to CSV file')

def main(args=None):
    rclpy.init(args=args)
    node = PoseSaver()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        # Ao interromper, salvar os dados em CSV
        node.save_to_csv()
        node.get_logger().info('Shutting down and saving data...')
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()
