{*
 * $Revision: 20954 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}

<div class="gbBlock gcBackground1">
  <h2> {g->text text="Slideshow"} </h2>
</div>

<div class="gbBlock">
  {if !empty($SlideshowApplet.NoProtocolError)}
  <div class="giError">
    {g->text text="This applet relies on a G2 module that is not currently enabled.  Please ask an administrator to enable the 'remote' module."}
  </div>
  {else}
  <object classid="clsid:8AD9C840-044E-11D1-B3E9-00805F499D93"
	  codebase="http://java.sun.com/products/plugin/autodl/jinstall-1_4-windows-i586.cab#Version=1,4,0,0"
	  width="300" height="450">
    <param name="code" value="{$SlideshowApplet.code}"/>
    <param name="archive" value="{g->url href="modules/slideshowapplet/applets/GalleryRemoteAppletMini.jar"},{g->url href="modules/slideshowapplet/applets/GalleryRemoteHTTPClient.jar"},{g->url href="modules/slideshowapplet/applets/applet_img.jar"}"/>
    <param name="type" value="application/x-java-applet;version=1.4"/>
    <param name="scriptable" value="false"/>
    <param name="progressbar" value="true"/>
    <param name="boxmessage" value="{g->text text="Downloading the Gallery Remote Applet"}"/>
    <param name="gr_url" value="{$SlideshowApplet.g2BaseUrl}"/>
    <param name="gr_cookie_name" value="{$SlideshowApplet.cookieName}"/>
    <param name="gr_cookie_value" value="{$SlideshowApplet.cookieValue}"/>
    <param name="gr_cookie_domain" value="{$SlideshowApplet.cookieDomain}"/>
    <param name="gr_cookie_path" value="{$SlideshowApplet.cookiePath}"/>
    <param name="gr_album" value="{$SlideshowApplet.album}"/>
    <param name="gr_user_agent" value="{$SlideshowApplet.userAgent}"/>
    <param name="gr_gallery_version" value="{$SlideshowApplet.galleryVersion}"/>
    <param name="gr_locale" value="{$SlideshowApplet.locale}"/>
    {foreach key=key item=value from=$SlideshowApplet.extra}
    <param name="{$key}" value="{$value}"/>
    {/foreach}
    {foreach key=key item=value from=$SlideshowApplet.default}
    <param name="GRDefault_{$key}" value="{$value}"/>
    {/foreach}
    {foreach key=key item=value from=$SlideshowApplet.override}
    <param name="GROverride_{$key}" value="{$value}"/>
    {/foreach}
    <param name="codebase_lookup" value="false"/>
    <param name="image" value="{g->url href="modules/slideshowapplet/applets/splash.gif"}"/>

    <comment>
      <embed
          type="application/x-java-applet;version=1.4"
          code="{$SlideshowApplet.code}"
          archive="{g->url href="modules/slideshowapplet/applets/GalleryRemoteAppletMini.jar"},{g->url href="modules/slideshowapplet/applets/GalleryRemoteHTTPClient.jar"},{g->url href="modules/slideshowapplet/applets/applet_img.jar"}"
          width="300"
          height="450"
          scriptable="false"
          progressbar="true"
          boxmessage="{g->text text="Downloading the Gallery Remote Applet"}"
          pluginspage="http://java.sun.com/j2se/1.4.2/download.html"
          gr_url="{$SlideshowApplet.g2BaseUrl}"
          gr_cookie_name="{$SlideshowApplet.cookieName}"
          gr_cookie_value="{$SlideshowApplet.cookieValue}"
          gr_cookie_domain="{$SlideshowApplet.cookieDomain}"
          gr_cookie_path="{$SlideshowApplet.cookiePath}"
          gr_album="{$SlideshowApplet.album}"
          gr_user_agent="{$SlideshowApplet.userAgent}"
          gr_gallery_version="{$SlideshowApplet.galleryVersion}"
          gr_locale="{$SlideshowApplet.locale}"
          {foreach key=key item=value from=$SlideshowApplet.extra}
          {$key}="{$value}"
          {/foreach}
          {foreach key=key item=value from=$SlideshowApplet.default}
          GRDefault_{$key}="{$value}"
          {/foreach}
          {foreach key=key item=value from=$SlideshowApplet.override}
          GROverride_{$key}="{$value}"
          {/foreach}
          codebase_lookup="false"
          image="{g->url href="modules/slideshowapplet/applets/splash.gif"}"
      >
          <noembed alt="{g->text text="Your browser doesn't support applets; you should use the standard slideshow."}">
            {g->text text="Your browser doesn't support applets; you should use the standard slideshow."}
          </noembed>
      </embed>
    </comment>
  </object>
  {/if}
</div>
