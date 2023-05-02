
#include <GLFW/glfw3.h>
#include <engine/Billboard.h>
#include <engine/CollisionBox.h>
#include <engine/Objectives.h>
#include <engine/Particles.h>
#include <engine/Plane.h>
#include <engine/QuadTexture.h>
#include <engine/RigidModel.h>
#include <engine/Terrain.h>
#include <engine/functions.h>
#include <engine/shader_m.h>
#include <engine/skybox.h>
#include <engine/textrenderer.h>
#include <glad/glad.h>
#include<ctime>
#include "ARTE.h"
#include"mod.h"

#include <iostream>
#include<Windows.h>


int main()
{
    //:::: INICIALIZAMOS GLFW CON LA VERSIÓN 3.3 :::://
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //:::: CREAMOS LA VENTANA:::://
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Relax Day", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    //:::: OBTENEMOS INFORMACIÓN DE TODOS LOS EVENTOS DE LA VENTANA:::://
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetJoystickCallback(joystick_callback);

    //:::: DESHABILITAMOS EL CURSOR VISUALMENTE EN LA PANTALLA :::://
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //:::: INICIALIZAMOS GLAD CON LAS CARACTERÍSTICAS DE OPENGL ESCENCIALES :::://
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //INICIALIZAMOS LA ESCENA
    Shader ourShader("shaders/multiple_lighting.vs", "shaders/multiple_lighting.fs");
    Shader selectShader("shaders/selectedShader.vs", "shaders/lighting_maps.fs");

    numerosaleato();
    initScene(ourShader);

    Terrain terrain("textures/hm3.jpg", texturePaths);
    SkyBox sky(1.0f, "10");
    cb = isCollBoxModel ? &models[indexCollBox].collbox : &collboxes.at(indexCollBox).second;

 
    //:::: RENDER:::://
  
    glfwSetTime(0);
    time1 = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        conteo();
        glfwSetWindowTitle(window, (("X:" + std::to_string(camera.Position.x) + ", " + "Y:" + std::to_string(camera.Position.y) + ", " + "Z:" + std::to_string(camera.Position.z) + "/Puntuacion: " + "70/" + std::to_string(jugador->getHojaspunt())).c_str()));
        //::::TIMING:::://Ayuda a crear animaciones fluidas
        float currentFrame = glfwGetTime();
        deltaTime = (currentFrame - lastFrame);
        lastFrame = currentFrame;
        respawnCount += 0.1;

        //::::ENTRADA CONTROL:::://
        processInput(window);
        //:::: LIMPIAMOS BUFFERS:::://
        glClearColor(0.933f, 0.811f, 0.647f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //:::: PASAMOS INFORMACIÓN AL SHADER:::://
        ourShader.use();

        //:::: DEFINICIÓN DE MATRICES::::// La multiplicaciónd e model*view*projection crea nuestro entorno 3D
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        //:::: RENDER DE MODELOS:::://
        drawModels(&ourShader, view, projection);

        if (jugador->getHojaspunt() == 8 || Esc->getArt() >= 1)
        {
            if (eleva >= -2)
                eleva -= 0.004;
            contador++;
            if (contador==1) {
                for (int i = 12; i < 154; i++)
                {
                    collboxes.erase(i);
                    collboxes.insert(pair<int, pair<string, CollisionBox>>(i, pair<string, CollisionBox>("null", null)));
                }
            }
        }
        else
        {
            if (eleva <= -0.5)
                eleva += 0.004;
        }

        //:::: SKYBOX, TERRENO Y LAGO:::://

        loadEnviroment(&terrain, &sky, view, projection);
        loadAnimations(view, projection);
        FinDelRecorrido(window);
        ourShader.use();
        drawMyModels(&ourShader);
        //:::: COLISIONES :::://
        collisions();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //:::: LIBERACIÓN DE MEMORIA:::://   
    delete[] texturePaths;
    
    delete jugador;
    

    delete Text;
    delete containermensaje;
    delete textDiagonal;
    delete Esc;
    delete Esc2;
    delete Pin;
    delete Mo;

   
   
 
    


    sky.Release();
    terrain.Release();
    for (int i = 0; i < lightcubes.size(); i++)
        lightcubes[i].second.Release();
    for (int i = 0; i < collboxes.size(); i++)
        collboxes[i].second.Release();
    for (int i = 0; i < models.size(); i++)
        models[i].Release();
    for (int i = 0; i < rigidModels.size(); i++)
    {
        physicsEnviroment.Unregister(rigidModels[i].getRigidbox());
    }

    physicsEnviroment.Unregister(&piso);
    physicsEnviroment.Unregister(&pared);
    glfwTerminate();

    return 0;
}

void initScene(Shader ourShader)
{
 

    //:::: DEFINIMOS LAS TEXTURAS DE LA MULTITEXTURA DEL TERRENO :::://
    texturePaths = new const char *[4];
    texturePaths[0] = "textures/multi1.jpg";
    texturePaths[1] = "textures/test3.png";
    texturePaths[2] = "textures/test1.png";
    texturePaths[3] = "textures/test2.png";

    //:::: POSICIONES DE TODAS LAS LUCES :::://
    pointLightPositions.push_back(glm::vec3(4.8, 5.2, 5));
    pointLightPositions.push_back(glm::vec3(1.3, 1.8, 4.5));
    pointLightPositions.push_back(glm::vec3(5, 6.8, 5));
    pointLightPositions.push_back(glm::vec3(0.0f, 10.0f, -3.0f));

    //:::: POSICIONES DE TODOS LOS OBJETOS CON FISICAS :::://
    physicsObjectsPositions.push_back(glm::vec3(0.0, 10.0, 0.0));
    physicsObjectsPositions.push_back(glm::vec3(2.0, 10.0, 0.0));
    physicsObjectsPositions.push_back(glm::vec3(1.0, 10.0, 0.0));
    physicsObjectsPositions.push_back(glm::vec3(-2.0, 10.0, -2.0));
    physicsObjectsPositions.push_back(glm::vec3(-2.0, 10.0, -2.0));
    physicsObjectsPositions.push_back(glm::vec3(15.0, 1.0, -2.0));
    physicsObjectsPositions.push_back(glm::vec3(15.0, 1.0, -2.0));
    physicsObjectsPositions.push_back(glm::vec3(25.0, 10.0, -2.0));


    //:::: ESTADO GLOBAL DE OPENGL :::://
    glEnable(GL_DEPTH_TEST);

    //Definimos los collbox de la camara
    camera.setCollBox();

    //:::: CARGAMOS LOS SHADERS :::://
    ourShader.use();
       
   /* Arbol = Billboard("textures/A1.png", (float)SCR_WIDTH, (float)SCR_HEIGHT, 1000.0f, 1400.0f);
    Arbol.setPosition(glm::vec3(-1.397f, 0.10f, 26.0f));
    Arbol.setScale(10.5f);
*/
   

    posi.push_back(glm::vec3(-8.52, 0.7, 15.13));    //1
    posi.push_back(glm::vec3(6.56, 0.7, 13.37));    //2
    posi.push_back(glm::vec3(4.52, 0.7, 15.66));  //3
    posi.push_back(glm::vec3(-2.82, 0.7, 13.70));   //4
    posi.push_back(glm::vec3(-5.43, 0.7, 13.81));   //5
    posi.push_back(glm::vec3(-18.47, 0.7, 14.88));     //6
    posi.push_back(glm::vec3(-19.07, 0.7, 12.78));        //7
    posi.push_back(glm::vec3(-7.96, 0.7, 17.28));      //8
    posi.push_back(glm::vec3(-11.79, 0.7, 12.23));     //9
    posi.push_back(glm::vec3(-7.91, 0.7, 12.73));       //10
    posi.push_back(glm::vec3(-19.96, 0.7, 15.32));       //11
    posi.push_back(glm::vec3(-4.72, 0.7, 12.87));      //12
    posi.push_back(glm::vec3(-4.68, 0.7, 24.61));     //13
    posi.push_back(glm::vec3(-1.81, 0.7, 23.99));       //14
    posi.push_back(glm::vec3(-1.61, 0.7, 25.35));       //15

    //:::::::::::::::::MI MODELOS::::::::::::::::::
    modelosclases();

    //:::: INICIALIZAMOS NUESTROS MODELOS :::://    

   
    
    //CREAMOS TODAS  LAS CAJAS DE COLISION INDIVIDUALES
    CollisionBox collbox;
    glm::vec4 colorCollbox(0.41f, 0.2f, 0.737f, 0.06f); 

    collbox = CollisionBox(glm::vec3(0, 1.5, -40), glm::vec3(100, 100, 0.3), colorCollbox);
    collboxes.insert(pair<int, pair<string, CollisionBox>>(0, pair<string, CollisionBox>("skyfront", collbox)));
    collbox = CollisionBox(glm::vec3(-40, 1.5, 0), glm::vec3(0.3, 100, 100), colorCollbox);
    collboxes.insert(pair<int, pair<string, CollisionBox>>(1, pair<string, CollisionBox>("skyleft", collbox)));
    collbox = CollisionBox(glm::vec3(40, 1.5, 0), glm::vec3(0.3, 100, 100), colorCollbox);
    collboxes.insert(pair<int, pair<string, CollisionBox>>(2, pair<string, CollisionBox>("skyright", collbox)));
    collbox = CollisionBox(glm::vec3(0, 1.5, 40), glm::vec3(100, 100, 0.3), colorCollbox);
    collboxes.insert(pair<int, pair<string, CollisionBox>>(3, pair<string, CollisionBox>("skyback", collbox)));
   
    //CREAMOS LOS LIGHTCUBES QUE ENREALIDAD SON COLLISION BOXES QUE NOS AYUDARAN A IDENTIFICAR LA POSICIÓN DE DONDE ESTAN
    glm::vec3 lScale(0.5);
    colorCollbox = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    collbox = CollisionBox(pointLightPositions[0], lScale, colorCollbox);
    lightcubes.insert(pair<int, pair<string, CollisionBox>>(0, pair<string, CollisionBox>("LUZ1", collbox)));
    collbox = CollisionBox(pointLightPositions[1], lScale, colorCollbox);
    lightcubes.insert(pair<int, pair<string, CollisionBox>>(1, pair<string, CollisionBox>("LUZ2", collbox)));
    collbox = CollisionBox(pointLightPositions[2], lScale, colorCollbox);
    lightcubes.insert(pair<int, pair<string, CollisionBox>>(2, pair<string, CollisionBox>("LUZ3", collbox)));
    collbox = CollisionBox(pointLightPositions[3], lScale, colorCollbox);
    lightcubes.insert(pair<int, pair<string, CollisionBox>>(3, pair<string, CollisionBox>("LUZ4", collbox)));
    
    //::::::::::::::AGUA::::::::::::::::::
    lago = Plane("textures/water.jpg", 90.0, 90.0, 200.0, 100.0);
    lago.setPosition(glm::vec3(0.0, -0.90, 0.0));
    lago.setAngles(glm::vec3(90.0, .0, 0.0));
    lago.setScale(glm::vec3(100.0));
     
}
//:::: CONFIGURACIONES :::://

void loadEnviroment(Terrain *terrain, SkyBox *sky, glm::mat4 view, glm::mat4 projection)
{
    glm::mat4 model = mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0, -2.5f, 0.0f));   // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(100.5f, 100.0f, 100.5f)); // it's a bit too big for our scene, so scale it down

    terrain->draw(model, view, projection);
    terrain->getShader()->setFloat("shininess", 100.0f);
    setMultipleLight(terrain->getShader(), pointLightPositions);

    glm::mat4 skyModel = mat4(1.0f);
    skyModel = glm::translate(skyModel, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    skyModel = glm::scale(skyModel, glm::vec3(40.0f, 40.0f, 40.0f));  // it's a bit too big for our scene, so scale it down
    sky->draw(skyModel, view, projection, skyPos);
    sky->getShader()->setFloat("shininess", 10.0f);
    setMultipleLight(sky->getShader(), pointLightPositions);

    //RENDER DE LIGHT CUBES
    if (renderLightingCubes)
        for (pair<int, pair<string, CollisionBox>> lights : lightcubes)
            lights.second.second.draw(view, projection);

    //CAMBIO DÍA NOCHE
  /* if ((float)glfwGetTime() >= 30)
    {
            if (oscu > 0.2) {
                mainLight = vec3(oscu);
                oscu -= 0.001;
            }
            else {
                if (cambiosky == 0)
                {
                    sky->reloadTexture("11");
                    cambiosky++;
                }
            }
    }*/ 
}
void drawModels(Shader *shader, glm::mat4 view, glm::mat4 projection)
{
    //DEFINIMOS EL BRILLO  DEL MATERIAL
    shader->setFloat("material.shininess", 60.0f);
    setMultipleLight(shader, pointLightPositions);   
    for (int i = 0; i < models.size(); i++)
    {
        //SI SE RECOGIO EL OBJETO
        shader->use();
        models[i].Draw(*shader);
        detectColls(&models[i].collbox, models[i].name, &camera, renderCollBox, collidedObject_callback);
    }
}
/*
void setSimpleLight(Shader *shader)
{
    shader->setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    shader->setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    shader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    shader->setInt("lightType", (int)lightType);
    shader->setVec3("light.position", lightPos);
    shader->setVec3("light.direction", lightDir);
    shader->setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
    shader->setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
    shader->setVec3("viewPos", camera.Position);
    shader->setFloat("light.constant", 0.2f);
    shader->setFloat("light.linear", 0.02f);
    shader->setFloat("light.quadratic", 0.032f);
    shader->setFloat("material.shininess", 60.0f);
}*/
void setMultipleLight(Shader *shader, vector<glm::vec3> pointLightPositions)
{
    shader->setVec3("viewPos", camera.Position);

    

 shader->setVec3("dirLights[0].direction", pointLightPositions[0]);
    shader->setVec3("dirLights[0].ambient", mainLight.x, mainLight.y, mainLight.z);
    shader->setVec3("dirLights[0].diffuse", mainLight.x, mainLight.y, mainLight.z);
    shader->setVec3("dirLights[0].specular", mainLight.x, mainLight.y, mainLight.z);

    shader->setVec3("dirLights[1].direction", pointLightPositions[1]);
    shader->setVec3("dirLights[1].ambient", 0.3f, 0.5f, 0.5f);
    shader->setVec3("dirLights[1].diffuse", 0.4f, 0.4f, 0.4f);
    shader->setVec3("dirLights[1].specular", 0.5f, 0.5f, 0.5f);


    shader->setVec3("dirLights[2].direction", pointLightPositions[2]);
    shader->setVec3("dirLights[2].ambient", 0.3f, 0.5f, 0.5f);
    shader->setVec3("dirLights[2].diffuse", 0.4f, 0.4f, 0.4f);
    shader->setVec3("dirLights[2].specular", 0.5f, 0.5f, 0.5f);

    shader->setVec3("dirLights[3].direction", pointLightPositions[3]);
    shader->setVec3("dirLights[3].ambient", 0.05f, 0.05f, 0.05f);
    shader->setVec3("dirLights[3].diffuse", 0.4f, 0.4f, 0.4f);
    shader->setVec3("dirLights[3].specular", 0.5f, 0.5f, 0.5f);

    shader->setVec3("pointLights[0].position", pointLightPositions[0]);
    shader->setVec3("pointLights[1].direction", glm::vec3(1.53, 1.9, 4.49));
    shader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    shader->setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    shader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("pointLights[0].constant", 1.0f);
    shader->setFloat("pointLights[0].linear", 0.09);
    shader->setFloat("pointLights[0].quadratic", 0.032);

    shader->setVec3("pointLights[1].position", pointLightPositions[1]);
    shader->setVec3("pointLights[1].direction", glm::vec3(1.53, 1.9, 4.49));
    shader->setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    shader->setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    shader->setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("pointLights[1].constant", 1.0f);
    shader->setFloat("pointLights[1].linear", 0.09);
    shader->setFloat("pointLights[1].quadratic", 0.032);

    glm::vec3 lightColor;
    lightColor.x = sin(glfwGetTime() * 2.0f);
    lightColor.y = sin(glfwGetTime() * 0.7f);
    lightColor.z = sin(glfwGetTime() * 1.3f);
    glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence

    shader->setVec3("pointLights[2].position", pointLightPositions[2]);
    shader->setVec3("pointLights[2].ambient", ambientColor);
    shader->setVec3("pointLights[2].diffuse", diffuseColor);
    shader->setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("pointLights[2].constant", 1.0f);
    shader->setFloat("pointLights[2].linear", 0.09);
    shader->setFloat("pointLights[2].quadratic", 0.032);

 /*   shader->setVec3("pointLights[2].position", pointLightPositions[2]);
    shader->setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    shader->setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    shader->setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("pointLights[2].constant", 1.0f);
    shader->setFloat("pointLights[2].linear", 0.09);
    shader->setFloat("pointLights[2].quadratic", 0.032);
*/
    shader->setVec3("pointLights[3].position", pointLightPositions[3]);
    shader->setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    shader->setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    shader->setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("pointLights[3].constant", 1.0f);
    shader->setFloat("pointLights[3].linear", 0.09);
    shader->setFloat("pointLights[3].quadratic", 0.032);

 shader->setVec3("spotLights[0].position", pointLightPositions[0]);
    shader->setVec3("spotLights[0].direction", glm::vec3(4.68f, 1.8f, 5.04f));
    shader->setVec3("spotLights[0].ambient", 0.0f, 0.0f, 0.0f);
    shader->setVec3("spotLights[0].diffuse", 1.0f, 1.0f, 1.0f);
    shader->setVec3("spotLights[0].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("spotLights[0].constant", 1.0f);
    shader->setFloat("spotLights[0].linear", 0.09);
    shader->setFloat("spotLights[0].quadratic", 0.032);
    shader->setFloat("spotLights[0].cutOff", glm::cos(glm::radians(12.5f)));
    shader->setFloat("spotLights[0].outerCutOff", glm::cos(glm::radians(15.0f)));

    // spotLight
    shader->setVec3("spotLights[1].position", pointLightPositions[1]);
    shader->setVec3("spotLights[1].direction", glm::vec3(1.53, 1.9, 4.49));
    shader->setVec3("spotLights[1].ambient", 0.0f, 0.0f, 0.0f);
    shader->setVec3("spotLights[1].diffuse", 1.0f, 1.0f, 1.0f);
    shader->setVec3("spotLights[1].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("spotLights[1].constant", 1.0f);
    shader->setFloat("spotLights[1].linear", 0.09);
    shader->setFloat("spotLights[1].quadratic", 0.032);
    shader->setFloat("spotLights[1].cutOff", glm::cos(glm::radians(12.5f)));
    shader->setFloat("spotLights[1].outerCutOff", glm::cos(glm::radians(15.0f)));

 /*  shader->setVec3("spotLights[2].position", pointLightPositions[2]);
    shader->setVec3("spotLights[2].direction", camera.Front);
    shader->setVec3("spotLights[2].ambient", 0.0f, 0.0f, 0.0f);
    shader->setVec3("spotLights[2].diffuse", 1.0f, 1.0f, 1.0f);
    shader->setVec3("spotLights[2].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("spotLights[2].constant", 1.0f);
    shader->setFloat("spotLights[2].linear", 0.09);
    shader->setFloat("spotLights[2].quadratic", 0.032);
    shader->setFloat("spotLights[2].cutOff", glm::cos(glm::radians(12.5f)));
    shader->setFloat("spotLights[2].outerCutOff", glm::cos(glm::radians(15.0f)));
*/
    shader->setVec3("spotLights[3].position", pointLightPositions[3]);
    shader->setVec3("spotLights[3].direction", camera.Front);
    shader->setVec3("spotLights[3].ambient", 0.0f, 0.0f, 0.0f);
    shader->setVec3("spotLights[3].diffuse", 1.0f, 1.0f, 1.0f);
    shader->setVec3("spotLights[3].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("spotLights[3].constant", 1.0f);
    shader->setFloat("spotLights[3].linear", 0.09);
    shader->setFloat("spotLights[3].quadratic", 0.032);
    shader->setFloat("spotLights[3].cutOff", glm::cos(glm::radians(12.5f)));
    shader->setFloat("spotLights[3].outerCutOff", glm::cos(glm::radians(15.0f)));
 
   
    shader->setInt("lightType", (int)lightType);
    shader->setInt("maxRenderLights", 3);
}

void loadAnimations(glm::mat4 view, glm::mat4 projection)
{

  
    //AGUA ANIMACIÓN
    if (AnimaY <= 4.0f && AnimaX <= 4.0f && !watterOut)
    {
        AnimaX -= 0.001f;
        AnimaY -= 0.001f;
    }else
    {
        watterOut = true;
        if (AnimaY > 0.0f && AnimaX > 0.0f && watterOut)
        {
            AnimaX -= 0.001f;
            AnimaY -= 0.001f;
        }
        else
            watterOut = false;
    }

    lago.draw(AnimaX, AnimaY, view, projection);


    //Animación de fuego
   
  /*  Arbol.Draw(camera, ArbolX, ArbolY);*/




    //::::::::::GUI::::::::::::::::
   


 
    //::::::::::TEXT:::::::::::::::
/*    textDiagonal->RenderText(diagonal, 0.62f, -0.87f, 0.004f, glm::vec3(0.0f, 0.0f, 0.0f));*/

}
void modelosclases() {
    jugador = new prota();
   
  
  
    
    Esc = new Art(Model("Torso", "models/Esc2/model.obj", glm::vec3(4.86999, 0.0300002, 7.4), glm::vec3(0, 0, 0), glm::vec3(0.5, 0.5, 0.5), 0.0f, initScale));
    p.push_back(0);
    Esc2 = new Art(Model("Leon", "models/Esc2/Leon.obj", glm::vec3(4.86999, -0.0399996, 2.0), glm::vec3(0, 0, 0), glm::vec3(0.5, 0.5, 0.5), 0.0f, initScale));
    p.push_back(0);
    Pin = new Art(Model("Cuadro", "models/Pintura/P1.obj", glm::vec3(4.86999, 0.46, 5.1), glm::vec3(0, -90, 0), glm::vec3(0.5, 0.5, 0.5), 0.0f, initScale));
    p.push_back(0);
  Mo = new Modelo(Model("mod", "models/Pintura/P1.obj", glm::vec3(-29.023f, 0.662f, 33.881f), glm::vec3(0, 0, 0), glm::vec3(0.5, 0.5, 0.5), 0.0f, initScale));
    p.push_back(0);
}
void drawMyModels(Shader* shader) {
    shader->setFloat("material.shininess", 60.0f);
    setMultipleLight(shader, pointLightPositions);
 

    
   
    //Escultura 1
 
    Esc->DrawArt(*shader);
    Esc->ArteSanima();
   

    //Escultura 2

    Esc2->DrawArt(*shader);
    Esc2->ArteSanima();


    // Pintura

    Pin->DrawArt(*shader);
    Pin->ArteSanima();
    Mo->Drawmod(*shader);
    Mo->modSanima();
    

    
  
    //COLISION 
    detectColls(&Esc->_mod.collbox, Esc->_mod.name, &camera, renderCollBox, collidedObject_callback);
  
  
}
void collisions()
{
    //TODO LO DE LAS COLISIONES VA AQUÍ

    detectColls(collboxes, &camera, renderCollBox, collidedObject_callback);
}
