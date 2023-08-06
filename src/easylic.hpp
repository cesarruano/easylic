/*
 * Copyright (c) 2023 Cesar Ruano
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
 
#ifndef __EASYLIC_H__
#define __EASYLIC_H__

#define EASYLIC_MAJOR_VERSION 1
#define EASYLIC_MINOR_VERSION 0
#define EASYLIC_PATCH_VERSION 0

#include <vector>
#include <string>
#include <chrono>  
#include <fstream>
#include <sstream>


namespace EasyLic{
    struct Attribute {
        std::string name;
        std::string value;
    };

    struct License_activation_requirements {
        bool needs_machine_uuid;
        bool needs_license_id;
        bool expires;
        std::string * license_id;
    };

    std::string execute_cmd(const std::string& cmd);

    std::string get_uuid(void);

    std::string get_executable_path(void);

    std::string get_current_time_in_seconds();

    int parse_string_to_int(const std::string& s);

    std::string generate_csv_from_attributes(const std::vector<Attribute>& attributes);

    std::vector<Attribute> generate_attributes_from_csv(const std::string& csv_data);

    std::string generate_validation_csv(long int * expiration_pt, std::string * uuid_pt, std::string * license_id_pt);

    Attribute* find_attribute(std::vector<Attribute>& attributes, const std::string& nameToFind);

    bool validate_csv_license(const std::string& csv_data, License_activation_requirements & lar);
    
    void generate_activation_file(long int * expiration_pt, std::string * uuid_pt, std::string * license_id_pt, std::string path_to_create);

    bool validate_activation_file(std::string license_id,  License_activation_requirements & lar, std::string path_to_activation);

}                             
#endif
