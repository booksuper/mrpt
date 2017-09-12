/* +---------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)               |
   |                          http://www.mrpt.org/                             |
   |                                                                           |
   | Copyright (c) 2005-2017, Individual contributors, see AUTHORS file        |
   | See: http://www.mrpt.org/Authors - All rights reserved.                   |
   | Released under BSD License. See details in http://www.mrpt.org/License    |
   +---------------------------------------------------------------------------+ */

/*---------------------------------------------------------------
    APPLICATION: kinect-rig-calib
    FILE: kinect-rig-calib_main.cpp
    AUTHOR: Eduardo Fernández-Moral <efernandezmoral@gmail.com>

    See README.txt for instructions or
          http://www.mrpt.org/Application:kinect-rig-calib
  ---------------------------------------------------------------*/

#include "kinect-rig-calib_misc.h"


pcl::PointCloud<PointT>::Ptr getPointCloud(const cv::Mat & rgb_img, const cv::Mat & depth_img, const mrpt::utils::TStereoCamera & calib)
{
    const int height = rgb_img.rows;
    const int width = rgb_img.cols;

    const float fx = 525.f;
    const float fy = 525.f;
    const float ox = 319.5f;
    const float oy = 239.5f;
//    const float fx = calib.rightCamera.fx();
//    const float fy = calib.rightCamera.fy();
//    const float ox = calib.rightCamera.cx();
//    const float oy = calib.rightCamera.cy();
    const float inv_fx = 1.f/fx;
    const float inv_fy = 1.f/fy;

    pcl::PointCloud<PointT>::Ptr pointCloudPtr (new pcl::PointCloud<PointT>());
    pointCloudPtr->height = height;
    pointCloudPtr->width = width;
    pointCloudPtr->is_dense = false;
    pointCloudPtr->points.resize(height*width);

    const float minDepth = 0.3;
    const float maxDepth = 10.0;
    if(depth_img.type() == CV_16U) // The image pixels are presented in millimetres
    {
        float minDepth_ = minDepth*1000;
        float maxDepth_ = maxDepth*1000;
//    #if ENABLE_OPENMP_MULTITHREADING_FrameRGBD
//    #pragma omp parallel for
//    #endif
        for( int y = 0; y < height; y++ )
        {
            unsigned short *_depth = const_cast<unsigned short*>(depth_img.ptr<unsigned short>(0)) + y*width;
            cv::Vec3b *_bgr = const_cast<cv::Vec3b*>(rgb_img.ptr<cv::Vec3b>(0)) + y*width;
            for( int x = 0; x < width; x++ )
            {
//                cv::Vec3b bgr = rgb_img.at<cv::Vec3b>(y,x);
//                pointCloudPtr->points[width*y+x].r = bgr[2];
//                pointCloudPtr->points[width*y+x].g = bgr[1];
//                pointCloudPtr->points[width*y+x].b = bgr[0];
                pointCloudPtr->points[width*y+x].r = (*_bgr)[2];
                pointCloudPtr->points[width*y+x].g = (*_bgr)[1];
                pointCloudPtr->points[width*y+x].b = (*_bgr++)[0];

                float z = (*_depth++)*0.001f;
//                    std::cout << "Build " << z << " from " << depth_img.at<unsigned short>(y,x) << std::endl;
                //if(z>0 && z>=minDepth_ && z<=maxDepth_) //If the point has valid depth information assign the 3D point to the point cloud
                if(z>=minDepth_ && z<=maxDepth_) //If the point has valid depth information assign the 3D point to the point cloud
                {
                    pointCloudPtr->points[width*y+x].x = (x - ox) * z * inv_fx;
                    pointCloudPtr->points[width*y+x].y = (y - oy) * z * inv_fy;
                    pointCloudPtr->points[width*y+x].z = z;
                }
                else //else, assign a NAN value
                {
//                        std::cout << z << " ";
//                        std::cout << depth_img.at<unsigned short>(y,x) << " " << std::endl;
                    pointCloudPtr->points[width*y+x].x = std::numeric_limits<float>::quiet_NaN ();
                    pointCloudPtr->points[width*y+x].y = std::numeric_limits<float>::quiet_NaN ();
                    pointCloudPtr->points[width*y+x].z = std::numeric_limits<float>::quiet_NaN ();
                }
            }
        }
    }
    else if(depth_img.type() == CV_32F) // The image pixels are presented in metres
    {
//        #if ENABLE_OPENMP_MULTITHREADING_FrameRGBD
//        #pragma omp parallel for
//        #endif
//            int non_zeros;
        for( int y = 0; y < height; y++ )
        {
            //float *_depth = depth_img.ptr<float>(0) + y*width;
            cv::Vec3b *_bgr = const_cast<cv::Vec3b*>(rgb_img.ptr<cv::Vec3b>(0)) + y*width;
            for( int x = 0; x < width; x++ )
            {
                pointCloudPtr->points[width*y+x].r = (*_bgr)[2];
                pointCloudPtr->points[width*y+x].g = (*_bgr)[1];
                pointCloudPtr->points[width*y+x].b = (*_bgr++)[0];

                //float z = *_depth++;
                float z = depth_img.at<float>(y,x); //convert from milimeters to meters
                //std::cout << "Build " << z << std::endl;
//                    std::cout << "Build " << z << " from " << depth_img.at<float>(y,x) << std::endl;
                //if(z>0 && z>=minDepth && z<=maxDepth) //If the point has valid depth information assign the 3D point to the point cloud

                if(z>=minDepth && z<=maxDepth) //If the point has valid depth information assign the 3D point to the point cloud
                {
                    pointCloudPtr->points[width*y+x].x = (x - ox) * z * inv_fx;
                    pointCloudPtr->points[width*y+x].y = (y - oy) * z * inv_fy;
                    //pointCloudPtr->points[width*y+x].x = -(y - oy) * z * inv_fy;
                    //pointCloudPtr->points[width*y+x].y = (x - ox) * z * inv_fx;
                    pointCloudPtr->points[width*y+x].z = z;
                    //std::cout << y << "x" << x << " pixel pt " << pointCloudPtr->points[width*y+x].x << " " << pointCloudPtr->points[width*y+x].y << " " << pointCloudPtr->points[width*y+x].z << std::endl;
//                        non_zeros++;
                }
                else //else, assign a NAN value
                {
//                        std::cout << z << " ";
                    pointCloudPtr->points[width*y+x].x = std::numeric_limits<float>::quiet_NaN ();
                    pointCloudPtr->points[width*y+x].y = std::numeric_limits<float>::quiet_NaN ();
                    pointCloudPtr->points[width*y+x].z = std::numeric_limits<float>::quiet_NaN ();
                }
            }
        }
//            std::cout << non_zeros << std::endl;
    }

    return pointCloudPtr;
}

pcl::PointCloud<PointT>::Ptr getPointCloudRegistered(const cv::Mat & rgb_img, const cv::Mat & depth_img, const mrpt::utils::TStereoCamera & calib, cv::Mat & depth_reg)
{
    const int height = rgb_img.rows;
    const int width = rgb_img.cols;

    const float fx = calib.leftCamera.fx();
    const float fy = calib.leftCamera.fy();
    const float ox = calib.leftCamera.cx();
    const float oy = calib.leftCamera.cy();
    const float inv_fx = 1.f/fx;
    const float inv_fy = 1.f/fy;

    const float fxc = calib.rightCamera.fx();
    const float fyc = calib.rightCamera.fy();
    const float oxc = calib.rightCamera.cx();
    const float oyc = calib.rightCamera.cy();
    //cout << "fx " << fx << " fxc " << fxc << endl;
    pcl::PointCloud<PointT>::Ptr pointCloudPtr (new pcl::PointCloud<PointT>());
    pointCloudPtr->height = height;
    pointCloudPtr->width = width;
    pointCloudPtr->is_dense = false;
    pointCloudPtr->points.resize(height*width);

    const float minDepth = 0.3;
    const float maxDepth = 10.0;
    if(depth_img.type() == CV_16U) // The image pixels are presented in millimetres
    {
        float minDepth_ = minDepth*1000;
        float maxDepth_ = maxDepth*1000;
//    #if ENABLE_OPENMP_MULTITHREADING_FrameRGBD
//    #pragma omp parallel for
//    #endif
        for( int y = 0; y < height; y++ )
        {
            unsigned short *_depth = const_cast<unsigned short*>(depth_img.ptr<unsigned short>(0)) + y*width;
            for( int x = 0; x < width; x++ )
            {
                float z = (*_depth++)*0.001f;
//                    std::cout << "Build " << z << " from " << depth_img.at<unsigned short>(y,x) << std::endl;
                //if(z>0 && z>=minDepth_ && z<=maxDepth_) //If the point has valid depth information assign the 3D point to the point cloud
                if(z>=minDepth_ && z<=maxDepth_) //If the point has valid depth information assign the 3D point to the point cloud
                {
                    pointCloudPtr->points[width*y+x].x = (x - ox) * z * inv_fx;
                    pointCloudPtr->points[width*y+x].y = (y - oy) * z * inv_fy;
                    pointCloudPtr->points[width*y+x].z = z;
                }
                else //else, assign a NAN value
                {
//                        std::cout << z << " ";
//                        std::cout << depth_img.at<unsigned short>(y,x) << " " << std::endl;
                    pointCloudPtr->points[width*y+x].x = std::numeric_limits<float>::quiet_NaN ();
                    pointCloudPtr->points[width*y+x].y = std::numeric_limits<float>::quiet_NaN ();
                    pointCloudPtr->points[width*y+x].z = std::numeric_limits<float>::quiet_NaN ();
                }
            }
        }
    }
    else if(depth_img.type() == CV_32F) // The image pixels are presented in metres
    {
//        #if ENABLE_OPENMP_MULTITHREADING_FrameRGBD
//        #pragma omp parallel for
//        #endif
//            int non_zeros;
        for( int y = 0; y < height; y++ )
        {
            //float *_depth = depth_img.ptr<float>(0) + y*width;
            for( int x = 0; x < width; x++ )
            {
                //float z = *_depth++;
                float z = depth_img.at<float>(y,x); //convert from milimeters to meters
                //std::cout << "Build " << z << std::endl;
//                    std::cout << "Build " << z << " from " << depth_img.at<float>(y,x) << std::endl;
                //if(z>0 && z>=minDepth && z<=maxDepth) //If the point has valid depth information assign the 3D point to the point cloud

                if(z>=minDepth && z<=maxDepth) //If the point has valid depth information assign the 3D point to the point cloud
                {
                    pointCloudPtr->points[width*y+x].x = (x - ox) * z * inv_fx;
                    pointCloudPtr->points[width*y+x].y = (y - oy) * z * inv_fy;
                    //pointCloudPtr->points[width*y+x].x = -(y - oy) * z * inv_fy;
                    //pointCloudPtr->points[width*y+x].y = (x - ox) * z * inv_fx;
                    pointCloudPtr->points[width*y+x].z = z;
                    //std::cout << y << "x" << x << " pixel pt " << pointCloudPtr->points[width*y+x].x << " " << pointCloudPtr->points[width*y+x].y << " " << pointCloudPtr->points[width*y+x].z << std::endl;
//                        non_zeros++;
                }
                else //else, assign a NAN value
                {
//                        std::cout << z << " ";
                    pointCloudPtr->points[width*y+x].x = std::numeric_limits<float>::quiet_NaN ();
                    pointCloudPtr->points[width*y+x].y = std::numeric_limits<float>::quiet_NaN ();
                    pointCloudPtr->points[width*y+x].z = std::numeric_limits<float>::quiet_NaN ();
                }
            }
        }
//            std::cout << non_zeros << std::endl;
    }

    pcl::PointCloud<PointT>::Ptr pointCloudPtr2 (new pcl::PointCloud<PointT>());
//    Eigen::Matrix4f pose_rgb2depth = Eigen::Matrix4f::Identity(); pose_rgb2depth(0,3) = -0.025;
    Eigen::Matrix4f pose_rgb2depth = getPoseEigen<float>(mrpt::poses::CPose3D(-calib.rightCameraPose));
    pcl::transformPointCloud(*pointCloudPtr, *pointCloudPtr2, pose_rgb2depth);
//    cout << "pose_rgb2depth \n" << pose_rgb2depth << endl;
//        int non_zero = 0;
//        cv::Mat depth_reg(depth_img.rows, depth_img.cols,CV_16UC1,cv::T(0));
    depth_reg = cv::Mat(rgb_img.rows, rgb_img.cols, CV_32FC1, cv::Scalar(0.0));

//        std::set<int> set_x, set_y;
//        std::set<int> set_pt;
    for( int y = 0; y < height; y++ )
    {
        for( int x = 0; x < width; x++ )
        {
            float x_, y_, z_;
            x_ = pointCloudPtr2->points[width*y+x].x;
            y_ = pointCloudPtr2->points[width*y+x].y;
            z_ = pointCloudPtr2->points[width*y+x].z;
//std::cout << "x_: " << x_ << " y_: " << y_ << " z_: " << z_ << std::endl;

            int x_coor = floor(x_*fxc/z_+oxc);
            int y_coor = floor(y_*fyc/z_+oyc);
//std::cout << x << " " << y << " " << x_coor << " " << y_coor << std::endl;
            if ((x_coor>=0)&&(x_coor<depth_reg.cols)&&(y_coor>=0)&&(y_coor<depth_reg.rows))
            {
                depth_reg.at<float>(y_coor,x_coor) = z_;//(ushort) (z_*1000.0);
                cv::Vec3b bgr = rgb_img.at<cv::Vec3b>(y_coor,x_coor);
                pointCloudPtr2->points[width*y+x].r = bgr[2];
                pointCloudPtr2->points[width*y+x].g = bgr[1];
                pointCloudPtr2->points[width*y+x].b = bgr[0];
            }
        }
    }

    return pointCloudPtr2;
}