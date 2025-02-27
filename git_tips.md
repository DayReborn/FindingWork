以下是一个 `tips.md` 的示例内容，涵盖日常 Git 和 GitHub 操作的常用命令及示例：

---

# Git & GitHub 日常使用小贴士

本文档提供日常使用 Git 和同步到 GitHub 的常用命令及示例。

---

## 基础配置

### 1. 配置用户信息
```bash
git config --global user.name "你的名字"
git config --global user.email "你的邮箱@example.com"
```

### 2. 生成 SSH 密钥（用于 GitHub 认证）
```bash
ssh-keygen -t ed25519 -C "你的邮箱@example.com"
# 将生成的公钥（~/.ssh/id_ed25519.pub）添加到 GitHub 的 SSH Keys 设置中
```

---

## 日常使用指令

### 1. 初始化仓库
```bash
git init
```

### 2. 克隆现有仓库
```bash
git clone https://github.com/username/repo.git  # HTTPS 方式
git clone git@github.com:username/repo.git     # SSH 方式
```

### 3. 添加文件到暂存区
```bash
git add .             # 添加所有修改
git add filename.txt  # 添加单个文件
```

### 4. 提交更改
```bash
git commit -m "提交说明"
```

### 5. 查看状态
```bash
git status
```

---

## 分支管理

### 1. 创建/切换分支
```bash
git branch                  # 查看本地分支
git branch new-branch       # 创建新分支
git checkout existing-branch # 切换分支
git checkout -b new-branch  # 创建并切换分支（推荐）
```

### 2. 合并分支
```bash
git merge branch-name      # 将 branch-name 合并到当前分支
```

### 3. 删除分支
```bash
git branch -d branch-name  # 删除本地分支
git push origin --delete branch-name  # 删除远程分支
```

---

## 同步到 GitHub

### 1. 添加远程仓库
```bash
git remote add origin git@github.com:username/repo.git
```

### 2. 推送到远程仓库
```bash
git push -u origin main         # 首次推送（设置上游分支）
git push                        # 后续推送（如果已设置上游分支）
git push origin branch-name    # 推送到特定分支
```

### 3. 拉取最新代码
```bash
git pull                      # 拉取并合并当前分支
git fetch origin              # 仅获取远程更新（不自动合并）
```

### 4. 处理冲突
1. 执行 `git pull` 后出现冲突
2. 手动编辑文件解决冲突（搜索 `<<<<<<<` 标记）
3. 重新提交：
   ```bash
   git add .
   git commit -m "解决冲突"
   git push
   ```

---

## 高级技巧

### 1. 撤销修改
```bash
git checkout -- filename.txt  # 撤销单个文件的未暂存修改
git reset --hard HEAD        # 丢弃所有未提交的修改（谨慎使用！）
```

### 2. 查看提交历史
```bash
git log --oneline --graph  # 简洁版提交历史
```

### 3. 储藏临时修改
```bash
git stash          # 临时保存修改
git stash pop      # 恢复最近一次储藏
```

### 4. 标签管理
```bash
git tag v1.0.0               # 创建标签
git push origin --tags       # 推送所有标签到远程
```

---

## 典型工作流程示例

```bash
# 开发新功能
git checkout -b feature/new-button
git add .
git commit -m "添加新的按钮组件"
git push origin feature/new-button

# 同步主分支更新
git checkout main
git pull origin main

# 合并分支并推送
git merge feature/new-button
git push origin main
```

---

## 注意事项
1. 频繁提交小修改，避免单次提交过大
2. 推送前先拉取最新代码（避免冲突）
3. 使用 `.gitignore` 文件排除不需要版本控制的文件
4. 避免将大文件（>100MB）提交到仓库

---

✅ 建议保存常用命令到备忘单，定期执行 `git status` 查看状态！