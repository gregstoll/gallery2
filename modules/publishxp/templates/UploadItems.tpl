{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<script type="text/javascript">
  // <![CDATA[
  setSubtitle("");
  setOnBackUrl("{g->url arg1="view=publishxp.Options" arg2="albumId=`$UploadItems.albumId`" arg3="setCaptions=`$UploadItems.setCaptions`" arg4="stripExtensions=`$UploadItems.stripExtensions`" htmlEntities=false}");
  setSubmitOnNext(true);
  setButtons(false, true, false);

  function publishItems() {ldelim}
    var xml = window.external.Property("TransferManifest");
    var files = xml.selectNodes("transfermanifest/filelist/file");

    for (i = 0; i < files.length; i++) {ldelim}
      var postTag = xml.createNode(1, "post", "");
      postTag.setAttribute("href", "{g->url arg1="controller=publishxp.UploadItems" htmlEntities=false forceFullUrl=true}");
      postTag.setAttribute("name", "userFile");

      var dataTag = xml.createNode(1, "formdata", "");
      dataTag.setAttribute("name", "MAX_FILE_SIZE");
      dataTag.text = "10000000";
      postTag.appendChild(dataTag);

      var dataTag = xml.createNode(1, "formdata", "");
      dataTag.setAttribute("name", "{g->formVar var="controller"}");
      dataTag.text = "publishxp.UploadItems";
      postTag.appendChild(dataTag);

      var dataTag = xml.createNode(1, "formdata", "");
      dataTag.setAttribute("name", "{g->formVar var="form[action][uploadItem]"}");
      dataTag.text = "1";
      postTag.appendChild(dataTag);

      var dataTag = xml.createNode(1, "formdata", "");
      dataTag.setAttribute("name", "{g->formVar var="form[albumId]"}");
      dataTag.text = "{$UploadItems.albumId}";
      postTag.appendChild(dataTag);

      var dataTag = xml.createNode(1, "formdata", "");
      dataTag.setAttribute("name", "{g->formVar var="form[setCaptions]"}");
      dataTag.text = "{$UploadItems.setCaptions}";
      postTag.appendChild(dataTag);

      var dataTag = xml.createNode(1, "formdata", "");
      dataTag.setAttribute("name", "{g->formVar var="form[stripExtensions]"}");
      dataTag.text = "{$UploadItems.stripExtensions}";
      postTag.appendChild(dataTag);

      files.item(i).appendChild(postTag);
    {rdelim}
    var uploadTag = xml.createNode(1, "uploadinfo", "");
    var htmluiTag = xml.createNode(1, "htmlui", "");

    htmluiTag.text = "{g->url arg1="view=core.ShowItem" arg2="itemId=`$UploadItems.albumId`" htmlEntities=false forceFullUrl=true}";
    uploadTag.appendChild(htmluiTag);

    xml.documentElement.appendChild(uploadTag);

    window.external.Property("TransferManifest")=xml;
    window.external.FinalNext();
  {rdelim}

  publishItems();
  // ]]>
</script>
