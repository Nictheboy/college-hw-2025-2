#!/usr/bin/python3

import os
import re
from datetime import datetime

# 获取脚本所在目录
base_dir = os.path.dirname(os.path.abspath(__file__))

# 获取所有作业文件夹
def get_homework_folders(base_dir):
    homework_folders = []
    for root, dirs, _ in os.walk(base_dir):
        for dir_name in dirs:
            # 匹配 HW_2025-03-25 或 HW_2025-03-25_1 形式
            match = re.match(r'HW_(\d{4}-\d{2}-\d{2})(?:_\d+)?$', dir_name)
            if match:
                # 检查是否存在 done 文件
                homework_path = os.path.join(root, dir_name)
                if os.path.exists(os.path.join(homework_path, 'done')):
                    continue
                    
                # 解析日期，明确指定完整日期格式
                date_str = match.group(1)
                date = datetime.strptime(date_str, "%Y-%m-%d")  # 解析为 YYYY-MM-DD 格式

                homework_folders.append({
                    'date': date,
                    'folder': os.path.relpath(homework_path, base_dir)  # 计算相对路径
                })
    
    return homework_folders

# 按日期排序并输出
def sort_homeworks(homework_folders):
    sorted_homeworks = sorted(homework_folders, key=lambda x: x['date'])

    # 输出排序结果
    for hw in sorted_homeworks:
        print(f"{hw['date'].strftime('%Y-%m-%d')} | {hw['folder']}")

# 运行主程序
if __name__ == "__main__":
    homework_folders = get_homework_folders(base_dir)
    if homework_folders:
        sort_homeworks(homework_folders)
    else:
        print("未找到任何作业文件夹")
