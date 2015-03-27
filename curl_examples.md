Some initial examples on using the API from the command line
------------------------------------------------------------

Pull a list of websites
```curl http://server-url/webfact/website.json```

Websites marked as public
```curl http://server-url/webfact/website.json?parameters[field_public]=0```

Pull just one, node 8
```curl http://server-url/webfact/website/8.json```


Create:

```curl -X POST -H "Content-Type: application/json" http://server-url/webfact/website -d '{ "node_title":"xxxxx from POST","hostname":"newxxx","site_email":"newxxx@test.com","public":"0","owner_uid":"27" }'```
<result>public is required</result>

Update node 72 (which does not exist and so fails)

```curl -X PUT -H "Content-Type: application/json" http://server-url/webfact/website/72 -d '{ "node_title":"xxxxx from PUT","hostname":"updatexxx","site_email":"updatexxx@test.com","public":0,"owner_uid":27 }'```
<?xml version="1.0" encoding="utf-8"?>
<result><result>0</result></result>

Update:

Update node 78 (which does exist: is this case all fields were replaced)
```curl -X PUT -H "Content-Type: application/json" http://server-url/webfact/website/78 -d '{ "node_title":"xxxxx from PUT","hostname":"updatexxx","site_email":"updatexxx@test.com","public":0,"owner_uid":27 }'```
<?xml version="1.0" encoding="utf-8"?>
<result><result>1</result></result>
 
Or updating just one field:
```curl -X PUT -H "Content-Type: application/json" http://server-url/webfact/website/78 -d '{"hostname":"updatex2x" }'```
<result><result>1</result></result>

