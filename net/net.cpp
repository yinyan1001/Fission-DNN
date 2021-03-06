#include<iostream>
#include<time.h>
#include <queue>
#include <set>
#include"math.h"
#include<algorithm>

#include"net.hpp"
#include"layers/layer.hpp"
#include"layers/dataLayer.hpp"
#include"layers/convLayer.hpp"
#include"layers/poolLayer.hpp"
#include"layers/activationLayer.hpp"
#include"layers/dropOutLayer.hpp"
#include"layers/hiddenLayer.hpp"
#include"layers/LRNLayer.hpp"
#include"layers/softMaxLayer.hpp"
#include"config/configBase.hpp"
#include"common/nDMatrix.hpp"

const bool DFS_TRAINING = false;
const bool DFS_TEST = false;
const bool FISS_TRAINING = false;

using namespace std;

// Create netWork
void createNet(const int rows, const int cols)
{
    Layer<float>* base_layer;
    const int layer_num = ConfigTable::getInstance()->getLayersNum();
    BaseLayerConfig* layer_config = ConfigTable::getInstance()->getFirstLayer();
    
    queue<BaseLayerConfig*>que;
    que.push(layer_config);
    set<BaseLayerConfig*>hash;
    hash.insert( layer_config );

    while(!que.empty()){
        layer_config = que.front();
        que.pop();
        if(string("DATA") == (layer_config->getType()))
        {
            base_layer = new DataLayer<float>(layer_config->getName(), rows, cols);
        }else if(string("CONV") == (layer_config->getType()))
        {
            base_layer = new ConvLayer<float>(layer_config->getName());
        }else if(string("POOLING") == (layer_config->getType()))
        {
            base_layer = new PoolLayer<float>(layer_config->getName());
        }else if(string("HIDDEN") == (layer_config->getType()))
        {
            base_layer = new HiddenLayer<float>(layer_config->getName());
        }else if(string("SOFTMAX") == (layer_config->getType()))
        {
           base_layer = new SoftMaxLayer<float>(layer_config->getName());
        }
        else if(string("ACTIVATION") == (layer_config->getType()))
        {
            base_layer = new ActivationLayer<float>(layer_config->getName());
        }else if(string("LRN") == (layer_config->getType()))
        {
           base_layer = new LRNLayer<float>(layer_config->getName());
        }else if(string("DROPOUT") == (layer_config->getType()))
        {
            base_layer = new DropOutLayer<float>(layer_config->getName());
        }

        LayerContainer<float>::getInstance()->storelayer(layer_config->getInput(), layer_config->getName(), base_layer);
        for(int i = 0; i < layer_config->getVecNext().size(); i++){
            if( hash.find( layer_config->getVecNext()[i]) == hash.end()){
                hash.insert( layer_config->getVecNext()[i]);
                que.push( layer_config->getVecNext()[i]);
            }
        }
    }
}

// Predict the result
void resultPredict(Phase phase)
{
    BaseLayerConfig* config = (BaseLayerConfig*) ConfigTable::getInstance()->getFirstLayer();
    queue<BaseLayerConfig*>que;
    que.push(config);
    set<BaseLayerConfig*>hash;
    hash.insert(config);
    while(!que.empty()){
        config = que.front();
        que.pop();
        Layer<float>* layer = (Layer<float>*)LayerContainer<float>::getInstance()->getLayerByName(config->getName());
        layer->Forward(phase);
        for(int i = 0; i < config->getVecNext().size(); i++){
            if( hash.find( config->getVecNext()[i] ) == hash.end()){
                hash.insert( config->getVecNext()[i] );
                que.push( config->getVecNext()[i] );
            }
        }
    }
}

//float dfsGetLearningRateReduce(configBase* config){
//    LayersBase* layer = (LayersBase*)Layers::instanceObject()->getLayer(config->_name);
//    if(config->_next.size() == 0){
//        layer->setRateReduce( 1 );
//        return 1;
//    }
//
//    float fRateReduce = 0;
//    for(int i = 0; i < config->_next.size(); i++){
//        fRateReduce += dfsGetLearningRateReduce( config->_next[i] );
//    }
//
//    layer->setRateReduce( fRateReduce );
//    printf("rate %f\n", layer->getRateReduce()); 
//
//    return fRateReduce;
//}
//
// Test netWork
void predictTestData(NDMatrix<float>& testSetX, NDMatrix<int>& testSetY)
{

    DataLayer<float>* data_layer = static_cast<DataLayer<float>*>(LayerContainer<float>::getInstance()->getLayerByName("data"));
    int batchSize = ConfigTable::getInstance()->getBatchSize();
    int example_size = testSetX.ND_num();

    for(int i = 0; i < ((example_size + batchSize - 1) / batchSize); i++)
    {
        data_layer->load_batch(i, testSetX, testSetY);
        resultPredict(TEST);
    }
}

// Linear structure training network
void getNetWorkCost()
{
    // forward
    resultPredict(TRAIN);
    //backward
    BaseLayerConfig* config = (BaseLayerConfig*) ConfigTable::getInstance()->getLastLayer();
    queue<BaseLayerConfig*>que;
    que.push(config);
    set<BaseLayerConfig*>hash;
    hash.insert(config);
    while(!que.empty()){
        config = que.front();
        que.pop();
        Layer<float>* layer = (Layer<float>*)LayerContainer<float>::getInstance()->getLayerByName(config->getName());
        layer->Backward();

        for(int i = 0; i < config->getVecPrev().size(); i++){
            if( hash.find( config->getVecPrev()[i] ) == hash.end()){
                hash.insert(config->getVecPrev()[i]);
                que.push(config->getVecPrev()[i]);
            }
        }
    }
}

std::vector<BaseLayerConfig*> g_vQue;
////std::map<LayersBase*, size_t> g_vFissNode;
//std::vector<SoftMaxLayer*> g_vBranchResult;
////int g_nMinCorrSize;
///*多少次迭代进行分裂*/
//int g_nSplitIndex = 15;
//int g_nCount = 0;
//
///* voting */
//void dfsResultPredict( configBase* config, cuMatrixVector<float>& testData, cuMatrix<int>*& testLabel, int nBatchSize)
//{
//    g_vQue.push_back( config );
//    if( config->_next.size() == 0 ){
//
//        DataLayer* datalayer = static_cast<DataLayer*>( Layers::instanceObject()->getLayer("data"));
//
//        for(int i = 0; i < (testData.size() + nBatchSize - 1) / nBatchSize; i++)
//        {
//            datalayer->getBatch_Images_Label(i , testData, testLabel);
//            for(int j = 0; j < g_vQue.size(); j++)
//            {
//                LayersBase* layer = (LayersBase*)Layers::instanceObject()->getLayer(g_vQue[j]->_name);
//                layer->forwardPropagation("test");
//                //                if(i == 0)
//                //                {
//                //                	cout<<layer->_name<<endl;
//                //                }
//                // is softmax, then vote
//                if( j == g_vQue.size() - 1 ){
//                    VoteLayer::instance()->vote( i , nBatchSize, layer->dstData );
//                }
//            }
//        }
//    }
//
//    for(int i = 0; i < config->_next.size(); i++){
//        configBase* tmpConfig = config->_next[i];
//        LayersBase* layer = (LayersBase*)Layers::instanceObject()->getLayer( config->_name );
//        layer->setCurBranchIndex(i);
//        dfsResultPredict( tmpConfig, testData, testLabel, nBatchSize );
//    }
//    g_vQue.pop_back();
//}
//
void dfsTraining(BaseLayerConfig* config, float nMomentum, NDMatrix<float>& trainData, NDMatrix<int>& trainLabel, int& iter)
{
    g_vQue.push_back(config);

    /*如果是一个叶子节点*/
    if (config->getVecNext().size() == 0){
        DataLayer<float>* datalayer = static_cast<DataLayer<float>*>(LayerContainer<float>::getInstance()->getLayerByName("data"));
        datalayer->random_load_batch(trainData, trainLabel);

        for(int i = 0; i < g_vQue.size(); i++){
            //printf("f %d %s\n", i, g_vQue[i]->_name.c_str());
            Layer<float>* layer = (Layer<float>*)LayerContainer<float>::getInstance()->getLayerByName(g_vQue[i]->getName());
            layer->Forward(TRAIN);
        }

        for( int i = g_vQue.size() - 1; i >= 0; i--){
            Layer<float>* layer = (Layer<float>*)LayerContainer<float>::getInstance()->getLayerByName(g_vQue[i]->getName());
            // if(layer->getRateReduce() > 1e-4){
            layer->Backward();
            //  }
            // else{
            //      break;
            //   }
        }
    }
    /*如果不是叶子节点*/
    for(int i = 0; i < config->getVecNext().size(); i++){
        BaseLayerConfig* tmpConfig = config->getVecNext()[i];
        Layer<float>* layer = (Layer<float>*)LayerContainer<float>::getInstance()->getLayerByName( config->getName() );
        //layer->setCurBranchIndex(i);
        dfsTraining( tmpConfig, nMomentum, trainData, trainLabel, iter);
    }
    g_vQue.pop_back();
}

///*
// *ascend order
// */
//bool cmp_ascend_Order( SoftMaxLayer* a, SoftMaxLayer* b)
//{
//    return (a->getCorrectNum()) < (b->getCorrectNum());
//}
//
///*
// *Get min result branch
// */
//void getBranchResult(LayersBase*curLayer)
//{
//    //叶子节点
//    if (curLayer->nextLayer.size() == 0)
//    {
//        SoftMaxLayer* tmp = (SoftMaxLayer*) curLayer;
//        g_vBranchResult.push_back(tmp);
//    }
//
//    for (int i = 0; i < curLayer->nextLayer.size(); i++) {
//        LayersBase* tmpLayer = curLayer->nextLayer[i];
//        getBranchResult(tmpLayer);
//    }
//    //	//叶子节点
//    //	if (curLayer->nextLayer.size() == 0)
//    //	{
//    //		SoftMaxLayer* tmp = (SoftMaxLayer*)curLayer;
//    //		if(tmp->getCorrectNum() < g_nMinCorrSize)
//    //		{
//    //			g_nMinCorrSize = tmp->getCorrectNum();
//    //			g_vMinBranch.push_back(tmp);
//    //		}
//    //	}
//    //
//    //	for(int i = 0; i < curLayer->nextLayer.size(); i++)
//    //	{
//    //		LayersBase* tmpLayer =  curLayer->nextLayer[i];
//    //		ascend_OrderBranch(tmpLayer);
//    //	}
//}
//
//
///*
// *Get Fissnode and Fission
// */
//void performFiss()
//{
//    for(int i = 0; i < g_vBranchResult.size(); i++)
//    {
//        LayersBase* tmpCur = (LayersBase*)g_vBranchResult[i];
//
//        /*Find fissable node*/
//        while (tmpCur->prevLayer[0]->_name != string("data") && tmpCur->prevLayer[0]->nextLayer.size() == 1)
//        {
//            tmpCur = tmpCur->prevLayer[0];
//        }
//        /*if curBranch is Fiss to data layer, then fiss another 如果分裂到数据层,那么就不再分裂,进行下一个分支的裂变*/
//        if (tmpCur->prevLayer[0]->_name== "data" && (i != g_vBranchResult.size() - 1))
//        continue;
//        else if(i == g_vBranchResult.size() - 1)
//        {
//            /*softmax layer fission, add a classifier*/
//            softmaxFission(g_vBranchResult[0]);
//            break;
//        }
//        else
//        {
//            string layerType = config::instanceObjtce()->getLayersByName(tmpCur->prevLayer[0]->_name)->_type;
//            /*全连接层和卷积层需要经过g_nSplitIndex迭代之后才裂变,其他层直接裂变*/
//            if(layerType == string("HIDDEN") || layerType == string("CONV"))
//            {
//                //cifar-10 set 15
//                g_nSplitIndex = 10;
//            }else
//            {
//                g_nSplitIndex = 1;
//            }
//            //Fission one Node every time
//            cout<< tmpCur->prevLayer[0]->_name<<endl;
//            NodeFission(tmpCur->prevLayer[0], tmpCur);
//            break;
//        }
//    }
//}
//
// Training netWork
void trainNetWork(NDMatrix<float>& trainSetX, NDMatrix<int>& trainSetY,
                  NDMatrix<float>& testSetX, NDMatrix<int>& testSetY)
{
    BaseLayerConfig* layer_config = ConfigTable::getInstance()->getFirstLayer();
    /*裂变学习率的初始设定*/
    //dfsGetLearningRateReduce( config );
    
    LOG(INFO) << "TestData Forecast The Result...";;
    predictTestData(testSetX, testSetY);
    cout<<endl;

    LOG(INFO) << "NetWork training......";
    int epochs = ConfigTable::getInstance()->getTrainEpochs();
    int iter_per_epo = ConfigTable::getInstance()->getIter_perEpoch();
    float momentum = ConfigTable::getInstance()->getMomentum();
    int layerNum = LayerContainer<float>::getInstance()->getLayersNum();
    int id = 0;

    DataLayer<float>* data_layer = static_cast<DataLayer<float>*>(LayerContainer<float>::getInstance()->getLayerByName("data"));

    clock_t start, stop;
    double runtime;

    start = clock();
    for(int epo = 0; epo < epochs; epo++)
    {
        clock_t inStart, inEnd;
        inStart = clock();
        if( false == DFS_TRAINING ){
            // Train network
            for(int iter = 0 ; iter < iter_per_epo; iter++)
            {
                data_layer->random_load_batch(trainSetX, trainSetY);
                getNetWorkCost();
            }
        }
        else{
            int iter = 0;
            g_vQue.clear();
            while(iter < iter_per_epo){
                dfsTraining(layer_config, momentum, trainSetX, trainSetY, iter);
                iter++;
            }
        }

        inEnd = clock();
//
//        config = (configBase*) config::instanceObjtce()->getFirstLayers();
//        //adjust learning rate
//        /*
//        queue<configBase*> que;
//        set<configBase*> hash;
//        hash.insert(config);
//        que.push(config);
//        while( !que.empty() ){
//            config = que.front();
//            que.pop();
//            LayersBase * layer = (LayersBase*)Layers::instanceObject()->getLayer(config->_name);
//            layer->adjust_learnRate(epo, FLAGS_lr_gamma, FLAGS_lr_power);
//
//            for(int i = 0; i < config->_next.size(); i++){
//                if( hash.find(config->_next[i]) == hash.end()){
//                    hash.insert(config->_next[i]);
//                    que.push(config->_next[i]);
//                }
//            }
//        }
//        */
//
        //**只调整三次学习率, 可修改
        if( epo == 150 || epo == 450 || epo == 750 ){
            BaseLayerConfig* config = (BaseLayerConfig*) ConfigTable::getInstance()->getFirstLayer();
            //adjust learning rate
            queue<BaseLayerConfig*> que;
            set<BaseLayerConfig*> hash;
            hash.insert(config);
            que.push(config);
            while( !que.empty() ){
                config = que.front();
                que.pop();
                Layer<float> * layer = (Layer<float>*)LayerContainer<float>::getInstance()->getLayerByName(config->getName());
                //layer->rateReduce();
                //**可修改
                if(epo == 150)
                    layer->setLrate(layer->getLrate()/10.0f);
                else if(epo == 450)
                    layer->setLrate(layer->getLrate()/5.0f);
                else layer->setLrate(layer->getLrate() / 2.0f);
                for(int i = 0; i < config->getVecNext().size(); i++){
                    if( hash.find(config->getVecNext()[i]) == hash.end()){
                        hash.insert(config->getVecNext()[i]);
                        que.push(config->getVecNext()[i]);
                    }
                }
            }
        }
        // Test network
        cout << "epochs: " << epo << " ,Time: " << (inEnd - inStart)/CLOCKS_PER_SEC<<"s,";
        if( DFS_TEST == false)
        {
            predictTestData( testSetX, testSetY);
        }
        else{
           //VoteLayer::instance()->clear();
           //static float fMax = 0;
           //BaseLayerConfig* config = (BaseLayerConfig*) ConfigTable::getInstance()->getFirstLayer();
           //dfsResultPredict(config, testData, testLabel, batchSize);
           //float fTest = VoteLayer::instance()->result();
           //if ( fMax < fTest ) fMax = fTest;
           //printf(" test_result %f/%f ", fTest, fMax);
        }
        cout << " ,Momentum: " << momentum << endl;
//
//        /*在进入下一次训练之前进行裂变*/
//        if (DFS_TRAINING == true && FISS_TRAINING == true )
//        {
//            g_nCount ++;
//            if (epo >= 40 && (epo % g_nSplitIndex) == 0 && (g_nCount >= g_nSplitIndex)) {
//                g_vBranchResult.clear();
//                LayersBase* curLayer = Layers::instanceObject()->getLayer("data");
//                getBranchResult(curLayer);
//                sort(g_vBranchResult.begin(), g_vBranchResult.end(), cmp_ascend_Order);
//                performFiss();
//                g_nCount = 0;
//            }
//        }
    }

    stop = clock();
    runtime = stop - start;
    cout<< epochs <<" epochs total rumtime is: "<<runtime /CLOCKS_PER_SEC<<" Seconds"<<endl;
}
