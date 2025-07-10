### 邻接矩阵
> 横纵坐标分别为所有的顶点，可以视为二维数组
##### 无向图

1. 完全关于主对角线对称
2. 主对角线全为0，有边则为1
<img src="image-7.png" alt="alt text" style="zoom:33%;" />

##### 有向图

1. 不一定关于主对角线对称，`横-纵`代表`起点-终点`，即`出-入`
2. 主对角线全为0，有边则为1
<img src="image-6.png" alt="alt text" style="zoom:33%;" />
#### 带权值
主对角线为0，有边则为权值，无边则为$∞$
<img src="image-5.png" alt="alt text" style="zoom:33%;" />



### 邻接表
> 形式为多条链表，条数为图的顶点数，即每个链表的头节点是图的每个顶点
  可视为一个数组，数组的每个元素是链表
##### 无向图
与邻接矩阵类似，也存在浪费一半空间的问题
<img src="image-4.png" alt="alt text" style="zoom:33%;" />

##### 有向图
只能表示出边
<img src="image-3.png" alt="alt text" style="zoom:33%;" />

##### 逆邻接表
与有向图的邻接表相对，只能表示入边。二者相加才是十字链表
<img src="image-2.png" alt="alt text" style="zoom:33%;" />



### 十字链表
> 用于高效存储有向图
  横向表示出边，纵向表示入边
##### 顶点结构
1. data：顶点数据
2. firstin：指向入边的边结点。指向入边，说明该顶点是入边的终点
3. firstout：指向出边的边结点。指向出边，说明该顶点是出边的起点
##### 边结构
1. tailvex：弧尾，即有向边的起点
2. headvex：弧头，即有向边的终点
3. headlink：指向弧头的下一个边结点，也就是说指向的那个边结点，其弧头是当前边的弧头
4. taillink：指向弧尾的下一个边结点，也就是说指向的那个边结点，其弧尾是当前边的弧尾
<img src="image-1.png" alt="alt text" style="zoom:33%;" />



### 邻接多重表
> 用于解决无向图空间浪费问题
##### 单位结构

1. ivex，jvex：某一条边连接的两个顶点的下标。因此，有几条边，就需要几个邻接多重表单元。由于是无向，ij的顺序无所谓
2. ilink：指向连接同顶点ivex的下一个单元的ivex位置
3. jlink：指向连接同顶点jvex的下一个单元的jvex位置
> 十字链表和临界多重表，每个指针位最多只能指向一个数据，以串联的形式组织起来

<img src="image.png" alt="alt text" style="zoom:33%;" align="left"/>