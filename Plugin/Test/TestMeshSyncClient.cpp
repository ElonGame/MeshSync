#include "pch.h"
#include "Test.h"
#include "MeshGenerator.h"
#include "../MeshSync/MeshSync.h"
using namespace mu;

static void Send(ms::Scene& scene)
{
    for (auto& obj : scene.objects) {
        if (auto *mesh = dynamic_cast<ms::Mesh*>(obj.get())) {
            mesh->setupFlags();
        }
    }

    ms::ClientSettings settings;
    ms::Client client(settings);

    {
        ms::FenceMessage mes;
        mes.type = ms::FenceMessage::FenceType::SceneBegin;
        client.send(mes);
    }
    {
        ms::SetMessage mes;
        mes.scene = scene;
        client.send(mes);
    }
    {
        ms::FenceMessage mes;
        mes.type = ms::FenceMessage::FenceType::SceneEnd;
        client.send(mes);
    }
}


TestCase(Test_SendMesh)
{
    for (int i = 0; i < 8; ++i) {
        ms::Scene scene;

        auto mesh = ms::Mesh::create();
        scene.objects.push_back(mesh);

        mesh->path = "/Test/Wave";
        mesh->refine_settings.flags.gen_normals = 1;
        mesh->refine_settings.flags.gen_tangents = 1;


        auto& points = mesh->points;
        auto& uv = mesh->uv0;
        auto& counts = mesh->counts;
        auto& indices = mesh->indices;
        auto& mids = mesh->material_ids;

        GenerateWaveMesh(counts, indices, points, uv, 2.0f, 1.0f, 32, 30.0f * mu::Deg2Rad * i);
        mids.resize(counts.size(), 0);

        Send(scene);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}


TestCase(Test_Animation)
{
    ms::Scene scene;
    {
        auto node = ms::Mesh::create();
        scene.objects.push_back(node);

        node->path = "/Test/Animation";
        node->position = { 0.0f, 0.0f, 0.0f };
        node->rotation = quatf::identity();
        node->scale = { 1.0f, 1.0f, 1.0f };
        GenerateIcoSphereMesh(node->counts, node->indices, node->points, node->uv0, 0.5f, 1);
        node->refine_settings.flags.gen_normals = 1;
        node->refine_settings.flags.gen_tangents = 1;
    }
    {
        auto clip = ms::AnimationClip::create();
        scene.animations.push_back(clip);

        auto anim = ms::TransformAnimation::create();
        clip->animations.push_back(anim);

        anim->path = "/Test/Animation";
        anim->translation.push_back({ 0.0f, {0.0f, 0.0f, 0.0f} });
        anim->translation.push_back({ 1.0f, {1.0f, 0.0f, 0.0f} });
        anim->translation.push_back({ 2.0f, {1.0f, 1.0f, 0.0f} });
        anim->translation.push_back({ 3.0f, {1.0f, 1.0f, 1.0f} });

        anim->rotation.push_back({ 0.0f, ms::rotateX(0.0f * mu::Deg2Rad) });
        anim->rotation.push_back({ 1.0f, ms::rotateX(90.0f * mu::Deg2Rad) });
        anim->rotation.push_back({ 2.0f, ms::rotateX(180.0f * mu::Deg2Rad) });
        anim->rotation.push_back({ 3.0f, ms::rotateX(270.0f * mu::Deg2Rad) });

        anim->scale.push_back({ 0.0f, {1.0f, 1.0f, 1.0f} });
        anim->scale.push_back({ 1.0f, {2.0f, 2.0f, 2.0f} });
        anim->scale.push_back({ 2.0f, {1.0f, 1.0f, 1.0f} });
        anim->scale.push_back({ 3.0f, {2.0f, 2.0f, 2.0f} });
    }
    Send(scene);
}


template<class color_t>
void CreateCheckerImage(RawVector<char>& dst, color_t black, color_t white, int width, int height)
{
    int num_pixels = width * height;
    int checker_size = 8;
    dst.resize_discard(num_pixels * sizeof(color_t));
    color_t *data = (color_t*)dst.data();
    for (int iy = 0; iy < height; ++iy) {
        for (int ix = 0; ix < width; ++ix) {
            bool cy = (iy / checker_size) % 2 == 0;
            bool cx = (ix / checker_size) % 2 == 0;
            if (cy)
                *data++ = cx ? white : black;
            else
                *data++ = cx ? black : white;
        }
    }
}

TestCase(Test_SendTexture)
{
    auto gen_id = []() {
        static int id_seed = 0;
        return ++id_seed;
    };

    // raw file textures
    {
        const char *raw_files[] = {
            "Texture_RGBA_u8.png",
            "Texture_RGBA_f16.exr",
        };

        ms::Scene scene;
        for (auto filename : raw_files) {
            RawVector<char> data;
            if (ms::FileToByteArray(filename, data)) {
                auto tex = ms::Texture::create();
                scene.textures.push_back(tex);
                tex->id = gen_id();
                tex->name = filename;
                tex->format = ms::TextureFormat::RawFile;
                tex->data = std::move(data);
            }
        }
        if (!scene.textures.empty())
            Send(scene);
    }

    {
        ms::Scene scene;

        const int width = 512;
        const int height = 512;
        {
            // RGBAu8
            unorm8x4 black{ 0.0f, 0.0f, 0.0f, 1.0f };
            unorm8x4 white{ 1.0f, 1.0f, 1.0f, 1.0f };

            RawVector<char> data;
            CreateCheckerImage(data, black, white, width, height);

            auto tex = ms::Texture::create();
            scene.textures.push_back(tex);
            tex->id = gen_id();
            tex->name = "RGBAu8";
            tex->format = ms::TextureFormat::RGBAu8;
            tex->width = width;
            tex->height = height;
            tex->data = std::move(data);
        }
        {
            // RGBAf16
            half4 black{ 0.0f, 0.0f, 0.0f, 1.0f };
            half4 white{ 1.0f, 1.0f, 1.0f, 1.0f };

            RawVector<char> data;
            CreateCheckerImage(data, black, white, width, height);

            auto tex = ms::Texture::create();
            scene.textures.push_back(tex);
            tex->id = gen_id();
            tex->name = "RGBAf16";
            tex->format = ms::TextureFormat::RGBAf16;
            tex->width = width;
            tex->height = height;
            tex->data = std::move(data);
        }
        {
            // RGBAf32
            float4 black{ 0.0f, 0.0f, 0.0f, 1.0f };
            float4 white{ 1.0f, 1.0f, 1.0f, 1.0f };

            RawVector<char> data;
            CreateCheckerImage(data, black, white, width, height);

            auto tex = ms::Texture::create();
            scene.textures.push_back(tex);
            tex->id = gen_id();
            tex->name = "RGBAf32";
            tex->format = ms::TextureFormat::RGBAf32;
            tex->width = width;
            tex->height = height;
            tex->data = std::move(data);
        }

        // material
        {
            auto mat = ms::Material::create();
            scene.materials.push_back(mat);
            mat->name = "TextMaterial1";
            mat->setColor({ 0.3f, 0.3f, 0.5f, 1.0f });
            mat->setEmission({ 0.7f, 0.1f, 0.2f, 1.0f });
            mat->setMetallic(0.2f);
            mat->setSmoothness(0.8f);
            mat->setColorMap(1);
            mat->setMetallicMap(5);
            mat->setEmissionMap(4);
        }

        Send(scene);
    }
}


TestCase(Test_Query)
{
    auto send_query = [](ms::QueryMessage::QueryType qt) {
        ms::ClientSettings settings;
        ms::Client client(settings);

        ms::QueryMessage query;
        query.type = qt;
        auto response = std::dynamic_pointer_cast<ms::ResponseMessage>(client.send(query));

        printf("querty: %d\n", (int)qt);
        printf("response:\n");
        if (response) {
            for (auto& t : response->text)
                printf("  %s\n", t.c_str());
        }
        else {
            printf("  (null)\n");
        }
    };

    send_query(ms::QueryMessage::QueryType::ClientName);
    send_query(ms::QueryMessage::QueryType::RootNodes);
    send_query(ms::QueryMessage::QueryType::AllNodes);
}
