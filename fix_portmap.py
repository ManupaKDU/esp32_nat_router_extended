import os

filepath = "src/urihandler/portmaphandler.c"
with open(filepath, "r") as f:
    content = f.read()

content = content.replace("""        if (strcmp(funcParam, "del") == 0)
        {
            delPortmapEntry(buf);
        }
    }
    httpd_resp_set_status(req, "302 Temporary Redirect");""", """        if (strcmp(funcParam, "del") == 0)
        {
            delPortmapEntry(buf);
        }
    }
    free(buf);
    httpd_resp_set_status(req, "302 Temporary Redirect");""")

with open(filepath, "w") as f:
    f.write(content)
