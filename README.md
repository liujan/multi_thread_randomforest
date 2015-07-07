# multi_thread_randomforest
多线程随机森林，分类树
默认开了8个线程，可以在Node.h的numOfThread改，一般为你电脑的CPU核心数
共有26个类别，分别是1，2，3..26 每条训练数据有617个特征。

训练文件名为train.csv， 内容格式为： id,value0,value1,...,value616,label, 其中id用于表示每一条训练数据，label表示该条数据的类别，value0表示第0个特征的值，value1表示第一个特征的值，其他依次类推。

测试文件为test.csv， 内容格式为： id,value0,value1,...,value616

预测结果保存于result.csv中， 内容格式为： id,label

我已经写好了makefile，和运行本程序所需要的命令run.sh，在linux下直接执行run.sh ./run.sh 就可以执行本程序了。 以上数据可在这个网站或得：https://inclass.kaggle.com/c/classification-sysu-2015/data

