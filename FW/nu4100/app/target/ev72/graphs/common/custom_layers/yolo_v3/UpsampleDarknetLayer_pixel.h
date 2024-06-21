

 template <typename pixel>  
 void UpsampleDarknetLayer_pixel<pixel>::Forward_cpu(const vector<Blob<pixel>*>& bottom,  
     const vector<Blob<pixel>*>& top) {  
   UpsampleDarknetParameter upsample_darknet_param = this->layer_param_.upsample_darknet_param();  
   int stride_ = upsample_darknet_param.stride();    
   
   int N = top[0]->shape(0);  
   int C = top[0]->shape(1);  
   int H = top[0]->shape(2);  
   int W = top[0]->shape(3);  
   
   const pixel *input = bottom[0]->cpu_data();  
   pixel *output = top[0]->mutable_cpu_data();  
   for (int n = 0; n < N; n++) {  
     for (int c = 0; c < C; c++) {  
       for (int h = 0; h < H; h++) {  
         for (int w = 0; w < W; w++) {  
           int nw = w/stride_;  
           int nh = h/stride_;  
           int out_idx = (((n * C + c) * H) + h) * W + w;  
           int in_idx = (((n * C + c) * (H / stride_)) + nh) * (W / stride_) + nw;  
           output[out_idx] = input[in_idx];  
         }  
       }  
     }  
   }  
 }  
