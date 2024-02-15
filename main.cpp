/*
In this program, we will sort through files inside folders within folders recursively and save the MIG results inside another excel file. 
*/

#include <iostream>
#include <filesystem>
#include <opencv4/opencv2/opencv.hpp>
#include "xlsxwriter.h"

/*
This function creates folders.

func: create_folders()
param: relative path with respect to build folder
return: void
*/
void create_folders(const std::string &path);


int main()
{
    std::string root_path = "../images";
    
    // Create a separate directory to save excel files
    const std::string mig_dir_path = "../MIG_results";
    create_folders(mig_dir_path);
    
    // To save names of the image files
    std::vector<std::string> file_names;

    // Checking if the parent folder exists
    if (!std::filesystem::exists(root_path) || !std::filesystem::is_directory(root_path))
    {
        std::cout << "The given path either does not exist or is not a directory.\n" << std::endl;
        return EXIT_FAILURE;
    }
    
    for (const auto &exp_dir: std::filesystem::directory_iterator(root_path)) // Iterating through root folder
    {
        // To save name of experiment folder
        std::string dir_name;

        // To create folder for MIG results for each experiment
        std::string mig_exp_dir;

        if (exp_dir.is_directory())
        {
            // Saves only name of the folder inside root, but not the full path 
            dir_name = exp_dir.path().filename().string();
        }

        /* Creating directories for MIG excel sheet */
        mig_exp_dir = mig_dir_path + "/" + dir_name;
        create_folders(mig_exp_dir);

        /* Sort filenames and create excel sheets */
        for (const auto &img_file_name: std::filesystem::directory_iterator(exp_dir)) // Iterating through image folder
        {
            if (img_file_name.is_regular_file())
            {
                // Contains only file names, not the full path
                file_names.push_back(img_file_name.path().filename().string());
            }
        }

        // Sorting through filenames
        std::sort(file_names.begin(), file_names.end(), [](const std::string &a, const std::string &b)
        {
            std::string num_a = a.substr(a.find_first_of("0123456789"));
            std::string num_b = b.substr(b.find_first_of("0123456789"));
            int int_a = std::stoi(num_a);
            int int_b = std::stoi(num_b);
            return int_a < int_b;
        });

        // Adding full path to the sorted list of filenames
        for (auto &file_name: file_names)
        {
            file_name = exp_dir.path().string() + "/" + file_name;
            std::cout << "/// File path                     :       " << file_name << std::endl;
        }

        // Creating an excel sheet
        std::string xlsx_path = mig_exp_dir + "/" + "MIG.xlsx" ;
        lxw_workbook *workbook = workbook_new(xlsx_path.c_str());
        lxw_worksheet *worksheet = workbook_add_worksheet(workbook, "MIG_Results");
        worksheet_write_string(worksheet, 0, 0, "MIG Single Frame", NULL);
        worksheet_write_string(worksheet, 0, 1, "Avg. MIG", NULL);

        int row_count = 1;
        double bucket = 0;
        double avg_mig;
        std::vector<double> mig_vector;
        for (const auto &file_name: file_names)
        {
            // Read the image in grayscale
            cv::Mat img = cv::imread(file_name, cv::IMREAD_GRAYSCALE);
            if (img.empty())
            {
                std::cout << "Input correct path to the image. Filename causing error: " << file_name << std::endl;
                return EXIT_FAILURE;
            }

            cv::Mat1f dx, dy, mag; // Declare matrices to store float values
            cv::Sobel(img, dx, CV_32F, 1, 0, 3); // Sobel operation for x-axis
            cv::Sobel(img, dy, CV_32F ,0, 1, 3); // Sobel operation for y-axis
            cv::magnitude(dx, dy, mag); // Calculate magnitude of gradient for each pixel of frame
            cv::Scalar total = cv::sum(mag); // Sum of magnitudes for all pixels
            double mig = total[0] / (img.rows * img.cols); // Average of magnitude for whole frame
            worksheet_write_number(worksheet, row_count, 0, mig, NULL); // Save the average to excel sheet
            mig_vector.push_back(mig);
            row_count++;
        }

        /* Summing up the MIG values for all frames and averaging it and adding it to the excel sheet*/
        for (const auto& mig_element: mig_vector)
        {
            bucket += mig_element;
        }
        avg_mig = bucket / static_cast<double>(mig_vector.size());
        worksheet_write_number(worksheet, 1, 1, avg_mig, NULL);
        
        file_names.clear();
        workbook_close(workbook);
        }
    
    return EXIT_SUCCESS;
}

void create_folders(const std::string &path)
{
    try
    {
        if (std::filesystem::exists(path))
        {
            std::cout << "\n////////////////////////////////////////////////////////////////////////////////////////\n" 
                      << "/// Folder already exists at path     :       " << path 
                      << "\n////////////////////////////////////////////////////////////////////////////////////////\n" 
                      << std::endl;
        } else
        {
            std::filesystem::create_directories(path);
            std::cout << "\n////////////////////////////////////////////////////////////////////////////////////////\n"
                      << "/// Folder created at path            :       " << path 
                      << "\n////////////////////////////////////////////////////////////////////////////////////////\n"
                      << std::endl;
        }
    } catch (const std::filesystem::filesystem_error &e)
    {
        std::cerr << "/// Error creating folder                 :       " << e.what() << std::endl; 
    }
}