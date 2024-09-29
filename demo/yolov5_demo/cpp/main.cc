// Copyright (c) 2023 by Rockchip Electronics Co., Ltd. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*-------------------------------------------
                Includes
-------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "yolov5.h"
#include "image_utils.h"
#include "file_utils.h"
#include "image_drawing.h"

//opencv
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <unistd.h>   // sleep()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>

#include <time.h>


#define FB_HIGHT   280
#define FB_WIDTH  240

/*-------------------------------------------
                  Main Function
-------------------------------------------*/
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("%s <model_path>\n", argv[0]);
        return -1;
    }

    const char *model_path = argv[1];
    const char *image_path = argv[2];


    clock_t start_time;
    clock_t end_time;
    char text[8];
    int width    = 640;
    int height   = 640;
    int channels = 3;
    int ret;
    rknn_app_context_t rknn_app_ctx;
    object_detect_result_list od_results;
    memset(&rknn_app_ctx, 0, sizeof(rknn_app_context_t));
    unsigned char *src_image = (unsigned char *)malloc(sizeof(unsigned char)*640*640*3);
    
    init_yolov5_model(model_path, &rknn_app_ctx);
    init_post_process();
    
    // Init opencv and fb
    int fb = open("/dev/fb0", O_RDWR);
    if(fb == -1)
    {
        close(fb);
        return -1;
    }
    size_t    screensize = FB_HIGHT * FB_WIDTH * 2;
    uint16_t* framebuffer = (uint16_t*)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);
    cv::Mat   rgb565Image(240, 240, CV_16UC1);
    
    cv::VideoCapture cap;
    cv::Mat bgr;
    cap.set(cv::CAP_PROP_FRAME_WIDTH,  240);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 240);
    cap.open(0); 

    while(1)
    {

        start_time = clock();
        cap >> bgr;
        //bgr = cv::imread("./bus.jpg");
        cv::resize(bgr, bgr, cv::Size(640,640), 0, 0, cv::INTER_LINEAR);
        for (int y = 0; y < height; ++y) {
          for (int x = 0; x < width; ++x) {
              cv::Vec3b pixel = bgr.at<cv::Vec3b>(y, x);
              src_image[(y * width + x) * channels + 0] = pixel[2]; // Red
              src_image[(y * width + x) * channels + 1] = pixel[1]; // Green
              src_image[(y * width + x) * channels + 2] = pixel[0]; // Blue
            }
        }

        memcpy(rknn_app_ctx.input_mems[0]->virt_addr, src_image,640*640*3);




        inference_yolov5_model(&rknn_app_ctx, &od_results);

        // Add rectangle and probability

        for (int i = 0; i < od_results.count; i++)
        {
            object_detect_result *det_result = &(od_results.results[i]);
            //  printf("%s @ (%d %d %d %d) %.3f\n", coco_cls_to_name(det_result->cls_id),
            //         det_result->box.left, det_result->box.top,
            //         det_result->box.right, det_result->box.bottom,
            //         det_result->prop);

            cv::rectangle(bgr,cv::Point(det_result->box.left ,det_result->box.top),
                              cv::Point(det_result->box.right,det_result->box.bottom),cv::Scalar(0,255,0),3);

            sprintf(text, "%s %.1f%%", coco_cls_to_name(det_result->cls_id), det_result->prop * 100);
            cv::putText(bgr,text,cv::Point(det_result->box.left, det_result->box.top - 8),
                                         cv::FONT_HERSHEY_SIMPLEX,1,
                                         cv::Scalar(0,255,0),2);
        
        }

        end_time = clock();
        float fps= (float) (CLOCKS_PER_SEC / (end_time - start_time)) ;

        sprintf(text,"fps=%.1f",fps); 
        cv::putText(bgr,text,cv::Point(0, 40),
                    cv::FONT_HERSHEY_SIMPLEX,1,
                    cv::Scalar(0,255,0),1);
        memset(text,0,8); 
        
        cv::resize(bgr, bgr, cv::Size(240,240), 0, 0, cv::INTER_LINEAR);
        int y_offset = 20;
        for (int i = 0; i < bgr.rows; ++i) {
            for (int j = 0; j < bgr.cols; ++j) {
                uint16_t b = (bgr.at<cv::Vec3b>(i, j)[0] >> 3) << 11;
                uint16_t g = (bgr.at<cv::Vec3b>(i, j)[1] >> 2) << 5;
                uint16_t r = (bgr.at<cv::Vec3b>(i, j)[2] >> 3);

                rgb565Image.at<uint16_t>(i, j) = ~ (r | g | b);
                framebuffer[ (i + y_offset) * FB_WIDTH + j] = rgb565Image.at<uint16_t>(i, j);
            }
        }
    }


    deinit_post_process();

    ret = release_yolov5_model(&rknn_app_ctx);
    if (ret != 0)
    {
        printf("release_yolov5_model fail! ret=%d\n", ret);
    }

    free(src_image);
    return 0;
}
