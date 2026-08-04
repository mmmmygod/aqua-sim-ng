# Aqua-Sim NG 水下网络协议研究

本仓库基于 [Aqua-Sim NG](https://github.com/rmartin5/aqua-sim-ng)，用于在 ns-3.41 中
实现和验证水下网络论文协议。所有成员使用同一个 `main` 作为基线；各自在独立分支
开发，完成后合并回 `main`。

> 仓库维护者首次使用此流程时，请先发布 `main`：
> `git switch main && git push -u origin main`。

## 1. 开发环境要求

- Linux 或 WSL2
- ns-3：`3.41`
- GCC/G++：11 或更高版本
- CMake：3.16 或更高版本
- Git、Python 3、Ninja（推荐）

Ubuntu / WSL2 可安装基础工具：

```bash
sudo apt update
sudo apt install -y build-essential cmake ninja-build python3 git pkg-config
```

## 2. 获取并初次构建 ns-3.41

```bash
mkdir -p ~/uwsn-workspace
cd ~/uwsn-workspace

wget https://www.nsnam.org/releases/ns-allinone-3.41.tar.bz2
tar -xjf ns-allinone-3.41.tar.bz2

cd ns-allinone-3.41/ns-3.41
./ns3 configure --build-profile=debug --enable-examples --enable-tests \
  --disable-python --disable-werror
./ns3 build
```

以下步骤均假定 ns-3 根目录为：

```text
~/uwsn-workspace/ns-allinone-3.41/ns-3.41
```

## 3. 拉取本仓库并构建

如果 `src/` 下还没有 `aqua-sim-ng`，直接克隆本仓库：

```bash
cd ~/uwsn-workspace/ns-allinone-3.41/ns-3.41/src
git clone --branch main https://github.com/mmmmygod/aqua-sim-ng.git aqua-sim-ng

cd ..
./ns3 configure --build-profile=debug --enable-examples --enable-tests \
  --disable-python --disable-werror
./ns3 build
```

模块放入 `src/aqua-sim-ng` 后必须重新执行一次 `./ns3 configure`，然后再 `./ns3 build`。

## 4. 已有官方 Aqua-Sim 时

如果你的目录中已经存在官方 Aqua-Sim：

```text
ns-3.41/src/aqua-sim-ng
```

无需重新下载 ns-3。将当前 Aqua-Sim 的 `origin` 改为本项目仓库，然后切换到本项目
的 `main`：

```bash
cd ~/uwsn-workspace/ns-allinone-3.41/ns-3.41/src/aqua-sim-ng

git remote set-url origin https://github.com/mmmmygod/aqua-sim-ng.git
git fetch origin
git switch main
git branch --set-upstream-to=origin/main main
git pull --ff-only

cd ../..
./ns3 configure --build-profile=debug --enable-examples --enable-tests \
  --disable-python --disable-werror
./ns3 build
```

此后 `origin` 指向本项目仓库，日常拉取和推送都使用它。

## 5. 开发流程

所有成员都按以下步骤操作：先更新 `main`，再从它新建自己的工作分支。分支格式为：

```text
research/<协议名或论文标识>/<工作内容>
```

示例：`research/dbr/energy-aware-forwarding`。

```bash
cd ~/uwsn-workspace/ns-allinone-3.41/ns-3.41/src/aqua-sim-ng

git switch main
git pull --ff-only origin main
git switch -c research/your-protocol/your-work-item

# 修改代码后，回到 ns-3 根目录构建和运行相关场景
cd ../..
./ns3 build

cd src/aqua-sim-ng
git add .
git commit -m "feat: describe the change"
```

将自己的工作分支推送到远端：

```bash
git push -u origin research/your-protocol/your-work-item
```

然后在 GitHub/GitLab 创建 Pull Request，将：

```text
research/your-protocol/your-work-item  →  main
```

代码通过构建、测试和审查后，再由负责人合并到 `main`。不要在自己的开发分支上直接
执行 `git push origin main`，否则容易覆盖或混入其他成员尚未审查的修改。

## 更新本仓库

```bash
cd ~/uwsn-workspace/ns-allinone-3.41/ns-3.41/src/aqua-sim-ng
git switch main
git pull --ff-only origin main

cd ../..
./ns3 build
```

若更新包含 `CMakeLists.txt` 或新增源文件，请先重新执行 `./ns3 configure`，再执行
`./ns3 build`。

请保留上游的版权和许可证文件；不要提交 `build/`、仿真输出或本地 IDE 配置。
