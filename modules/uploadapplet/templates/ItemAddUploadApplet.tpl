{*
 * $Revision: 17486 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
  {if !empty($ItemAddUploadApplet.NoProtocolError)}
  <div class="giError">
    {g->text text="The upload applet relies on a G2 module that is not currently enabled.  Please ask an administrator to enable the 'remote' module."}
  </div>
  {else}
  <form name="form1">
  <object classid="clsid:8AD9C840-044E-11D1-B3E9-00805F499D93"
	  codebase="http://java.sun.com/products/plugin/autodl/jinstall-1_4-windows-i586.cab#Version=1,4,0,0"
	  width="600" height="400" mayscript="true" id="applet_object">
    <param name="code" value="com.gallery.GalleryRemote.GRAppletMini"/>
    <param name="archive" value="{g->url href="modules/uploadapplet/applets/GalleryRemoteAppletMini.jar},{g->url href="modules/uploadapplet/applets/GalleryRemoteHTTPClient.jar"},{g->url href="modules/uploadapplet/applets/applet_img.jar"}"/>
    <param name="type" value="application/x-java-applet;version=1.4"/>
    <param name="scriptable" value="false"/>
    <param name="mayscript" value="false"/>
    <param name="progressbar" value="true"/>
    <param name="boxmessage" value="{g->text text="Downloading the Gallery Remote Applet"}"/>
    <param name="gr_url" value="{$ItemAddUploadApplet.g2BaseUrl}"/>
    <param name="gr_cookie_name" value="{$ItemAddUploadApplet.cookieName}"/>
    <param name="gr_cookie_value" value="{$ItemAddUploadApplet.cookieValue}"/>
    <param name="gr_cookie_domain" value="{$ItemAddUploadApplet.cookieDomain}"/>
    <param name="gr_cookie_path" value="{$ItemAddUploadApplet.cookiePath}"/>
    <param name="gr_album" value="{$ItemAddUploadApplet.album}"/>
    <param name="gr_user_agent" value="{$ItemAddUploadApplet.userAgent}"/>
    <param name="gr_gallery_version" value="{$ItemAddUploadApplet.galleryVersion}"/>
    <param name="gr_locale" value="{$ItemAddUploadApplet.locale}"/>
    {foreach key=key item=value from=$ItemAddUploadApplet.default}
    <param name="GRDefault_{$key}" value="{$value}"/>
    {/foreach}
    {foreach key=key item=value from=$ItemAddUploadApplet.override}
    <param name="GROverride_{$key}" value="{$value}"/>
    {/foreach}
    <param name="codebase_lookup" value="false"/>
    <param name="image" value="{g->url href="modules/slideshowapplet/applets/splash.gif"}"/>

    <comment>
      <embed
          type="application/x-java-applet;version=1.4"
          code="com.gallery.GalleryRemote.GRAppletMini"
	  id="applet_embed"
          archive="{g->url href="modules/uploadapplet/applets/GalleryRemoteAppletMini.jar},{g->url href="modules/uploadapplet/applets/GalleryRemoteHTTPClient.jar"},{g->url href="modules/uploadapplet/applets/applet_img.jar"}"
          width="600"
          height="400"
          mayscript="true"
          progressbar="true"
          boxmessage="{g->text text="Downloading the Gallery Remote Applet"}"
          pluginspage="http://java.sun.com/j2se/1.4.2/download.html"
          gr_url="{$ItemAddUploadApplet.g2BaseUrl}"
          gr_cookie_name="{$ItemAddUploadApplet.cookieName}"
          gr_cookie_value="{$ItemAddUploadApplet.cookieValue}"
          gr_cookie_domain="{$ItemAddUploadApplet.cookieDomain}"
          gr_cookie_path="{$ItemAddUploadApplet.cookiePath}"
          gr_album="{$ItemAddUploadApplet.album}"
          gr_user_agent="{$ItemAddUploadApplet.userAgent}"
          gr_gallery_version="{$ItemAddUploadApplet.galleryVersion}"
          gr_locale="{$ItemAddUploadApplet.locale}"
          {foreach key=key item=value from=$ItemAddUploadApplet.default}
          GRDefault_{$key}="{$value}"
          {/foreach}
          {foreach key=key item=value from=$ItemAddUploadApplet.override}
          GROverride_{$key}="{$value}"
          {/foreach}
          codebase_lookup="false"
          image="{g->url href="modules/slideshowapplet/applets/splash.gif"}"
      >
          <noembed alt="{g->text text="Your browser doesn't support applets; you should use one of the other upload methods."}">
            {g->text text="Your browser doesn't support applets; you should use one of the other upload methods."}
          </noembed>
      </embed>
    </comment>
  </object>
  </form>
  {/if}
</div>

<div id="uploadapplet_Feedback">
</div>

{g->addToTrailer}
<script type="text/javascript">
  // <![CDATA[
  var text = '', textUrls = '', apphead = '', appfoot = '';

  function startingUpload() {ldelim}
    text = textUrls = '';
    apphead = '<h2>{g->text text="Uploading files..." forJavascript=true}</h2><ul>';
    appfoot = '</ul>';
    addText('');
  {rdelim}

  function uploadedOne(itemId, itemName) {ldelim}
    addTextUrls('<li>' + itemName + '</li>',
	'<li><a href="{g->url arg1="view=core.ShowItem" arg2="itemId=" forceSessionId=false}'
	+ itemId + '">' + itemName + '</a></li>');
  {rdelim}

  function doneUploading() {ldelim}
    apphead = '<h2>{g->text text="Upload complete" forJavascript=true}</h2><ul>';
    appfoot = '</ul><p>{g->text text="You can keep uploading or go to some of the pictures you uploaded by clicking on the links above" forJavascript=true}</p>';

    showUrls();
  {rdelim}

  {literal}
  function addTextUrls(s, s1) {
    text = text + s;
    textUrls = textUrls + s1;

    getRef().innerHTML = '<div class="gbBlock gcBackground1">' + apphead
			 + text + appfoot + '</div>';
  }

  function addText(s) {
    text = text + s;
    textUrls = textUrls + s;

    getRef().innerHTML = '<div class="gbBlock gcBackground1">' + apphead
			 + text + appfoot + '</div>';
  }

  function showUrls() {
    getRef().innerHTML = '<div class="gbBlock gcBackground1">' + apphead
			 + textUrls + appfoot + '</div>';
  }

  function getRef() {
    if (document.getElementById) {
      return document.getElementById("uploadapplet_Feedback");
    } else if (document.all) {
      return document.all["uploadapplet_Feedback"];
    }
  }

  // Something is buggy in firefox, it tries to call "nullFindProxyForURL" and
  // all js calls after that do nothing.. define the function so our js works.
  function nullFindProxyForURL() { }
  {/literal}
  // ]]>
</script>
{/g->addToTrailer}
