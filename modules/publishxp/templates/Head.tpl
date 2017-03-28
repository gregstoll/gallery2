{*
 * $Revision: 17417 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<script type="text/javascript">
// <![CDATA[
  var onBackUrl = null;
  var submitOnNext = false;
  var title="{g->text text="Upload to Gallery" forJavascript=true}";

  {*
   * If you enable XpDebug, you get Back/Next buttons for all browsers, not
   * just the Windows Publishing Wizard.  It's useful for debugging in a development
   * environment, but should be disabled for production environments.
   *}
  {assign var=XpDebug value=false}
  {if ($XpDebug)}
  {literal}
  function MockWizard() {
	this.SetWizardButtons = function x(backButton, nextButton, finishButton) { }
	this.SetHeaderText = function y(title, subTitle) { }
	this.FinalBack = function z() { }
  }

  if (window.external) {
    impl = window.external;
  } else {
    impl = new MockWizard();
  }
  {/literal}
  {else}
  impl = window.external;
  {/if}

  {literal}
  function setOnBackUrl(url) {
    onBackUrl = url;
  }

  function setButtons(backButton, nextButton, finishButton) {
    impl.SetWizardButtons(backButton, nextButton, finishButton);
  }

  function setSubtitle(subTitle) {
    impl.SetHeaderText(title, subTitle);
  }

  function OnBack() {
    if (onBackUrl) {
      window.location.href = onBackUrl;
      setButtons(false, true, false);
    } else {
      impl.FinalBack();
    }
  }

  function setSubmitOnNext(boolValue) {
    submitOnNext = boolValue;
  }

  function OnNext() {
    if (submitOnNext) {
      document.getElementById('publishXpForm').submit();
    }
  }
  {/literal}
// ]]>
</script>

{if ($XpDebug)}
<input type="button" onclick="OnBack()" value="< Back">
<input type="button" onclick="OnNext()" value="Next >">
{/if}
