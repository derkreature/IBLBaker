//------------------------------------------------------------------------------------//
//                                                                                    //
//    ._____________.____   __________         __                                     //
//    |   \______   \    |  \______   \_____  |  | __ ___________                     //
//    |   ||    |  _/    |   |    |  _/\__  \ |  |/ // __ \_  __ \                    //
//    |   ||    |   \    |___|    |   \ / __ \|    <\  ___/|  | \/                    //
//    |___||______  /_______ \______  /(____  /__|_ \\___  >__|                       //
//                \/        \/      \/      \/     \/    \/                           //
//                                                                                    //
//    IBLBaker is provided under the MIT License(MIT)                                 //
//    IBLBaker uses portions of other open source software.                           //
//    Please review the LICENSE file for further details.                             //
//                                                                                    //
//    Copyright(c) 2014 Matt Davidson                                                 //
//                                                                                    //
//    Permission is hereby granted, free of charge, to any person obtaining a copy    //
//    of this software and associated documentation files(the "Software"), to deal    //
//    in the Software without restriction, including without limitation the rights    //
//    to use, copy, modify, merge, publish, distribute, sublicense, and / or sell     //
//    copies of the Software, and to permit persons to whom the Software is           //
//    furnished to do so, subject to the following conditions :                       //
//                                                                                    //
//    1. Redistributions of source code must retain the above copyright notice,       //
//    this list of conditions and the following disclaimer.                           //
//    2. Redistributions in binary form must reproduce the above copyright notice,    //
//    this list of conditions and the following disclaimer in the                     //
//    documentation and / or other materials provided with the distribution.          //
//    3. Neither the name of the copyright holder nor the names of its                //
//    contributors may be used to endorse or promote products derived                 //
//    from this software without specific prior written permission.                   //
//                                                                                    //
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      //
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        //
//    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE      //
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          //
//    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   //
//    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN       //
//    THE SOFTWARE.                                                                   //
//                                                                                    //
//------------------------------------------------------------------------------------//

#include <IblApplication.h>
#include <direct.h>

int main(int argc, char* argv[])
{
    ApplicationHandle applicationInstance = nullptr;

    // Move up one directory to get to the sandbox location.
    _chdir("../");
#if _WIN32
    applicationInstance = GetModuleHandle(nullptr);
#else 
    LOG ("Platform is not yet implemented");
    assert(0)
#endif

    assert (applicationInstance);
    std::unique_ptr<Ctr::IBLApplication> application;
    application.reset(new Ctr::IBLApplication(applicationInstance));
    bool encounteredError = false;

    if (application)
    {
        // Load parameters from data/parameters.xml
        if (application->loadParameters())
        {
            // Parse options and override parameters.
            if (application->parseOptions(argc, argv))
            {
                try
                {
                    // Initialization failure will throw std::runtime_error on failure.
                    application->initialize();

                    // Run failure will throw std::runtime_error on error.
                    application->run();
                }
                catch (const std::runtime_error& error)
                {
                    // Errored out, attempt to exit.
                    LOG("Something terrible happened: " << error.what());
                    return 0;
                }
            }
            // Save out application settings
            application->saveParameters();
        }
        // Destroy the application.
        LOG("Destroying the application");
        application.reset();
    }
    LOG("Shutting down application");
    return 0;
}
