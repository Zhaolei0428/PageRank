# PageRank
一个PageRank算法的实现，从urltxt读取url之间的关系，用Ell+COO的Hybrid稀疏矩阵存储，计算出各url的pagerank，将前十输出到top10.txt。
PageRank算法是修正的算法，即浏览每个网页都有a几率随机选取另一个网页，a是阻尼系数，取0.15.
